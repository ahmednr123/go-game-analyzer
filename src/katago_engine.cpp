#include "katago_engine.hpp"
#include "error.hpp"
#include <iostream>
#include <SDL3/SDL_log.h>
#include <filesystem>
#include <cmath>
#include <optional>

using json = nlohmann::json;

bool doesKataGoExist (std::string katago_path) {
#ifndef WINDOWS
    std::string cmd = "command -v " + katago_path + " /dev/null 2>&1";
#else
    std::string cmd = katago_path;
#endif

    return system(cmd.c_str()) == 0;
}

#ifndef WINDOWS
static void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
#endif

KataGoEngine::KataGoEngine(
    const std::string& katagoPath,
    const std::string& configPath,
    const std::string& modelPath,
    std::function<void(bool)> init_callback
) {
    if (!doesKataGoExist(katagoPath)) {
        is_init_failure = true;
        init_callback(is_init_failure);
        SDL_Log("Failed because katago doesnt exist");
        return;
    }

    if (!std::filesystem::exists(configPath)
            || !std::filesystem::exists(modelPath)
    ) {
        is_init_failure = true;
        init_callback(is_init_failure);
        SDL_Log("Failed because katago config or model doesnt exit");
        return;
    }

    startProcess(katagoPath, configPath, modelPath);
    readerThread = std::thread(&KataGoEngine::readerLoop, this);
}

KataGoEngine::~KataGoEngine() {
    running = false;

    if(readerThread.joinable())
        readerThread.join();

#ifndef WINDOWS
    if(childPid > 0) {
        kill(childPid, SIGTERM);
        waitpid(childPid, nullptr, 0);
    }
#else
    if (pi.hProcess) {
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    if (hChildStdinWr) CloseHandle(hChildStdinWr);
    if (hChildStdoutRd) CloseHandle(hChildStdoutRd);
#endif
}

void KataGoEngine::startProcess(
    const std::string& katagoPath,
    const std::string& configPath,
    const std::string& modelPath
) {
#ifndef WINDOWS
    int toChild[2], fromChild[2];
    pipe(toChild);
    pipe(fromChild);

    childPid = fork();
    if(childPid == 0) {
        // CHILD
        dup2(toChild[0], STDIN_FILENO);
        dup2(fromChild[1], STDOUT_FILENO);

        close(toChild[1]);
        close(fromChild[0]);

        execlp(katagoPath.c_str(), katagoPath.c_str(),
               "analysis",
               "-config", configPath.c_str(),
               "-model", modelPath.c_str(),
               nullptr);

        perror("exec failed");
        exit(1);
    }

    // PARENT
    inWriteFd = toChild[1];
    outReadFd = fromChild[0];

    close(toChild[0]);
    close(fromChild[1]);

    setNonBlocking(outReadFd);
#else
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;

    HANDLE hChildStdinRd, hChildStdoutWr;

    // --- Create pipes ---
    CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &sa, 0);
    SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);

    CreatePipe(&hChildStdinRd, &hChildStdinWr, &sa, 0);
    SetHandleInformation(hChildStdinWr, HANDLE_FLAG_INHERIT, 0);

    // --- Build command ---
    std::string cmd =
        "\"" + katagoPath + "\" analysis "
        "-config \"" + configPath + "\" "
        "-model \"" + modelPath + "\"";

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError  = hChildStdoutWr;
    si.hStdInput  = hChildStdinRd;

    // --- Launch process ---
    BOOL ok = CreateProcessA(
        NULL,
        cmd.data(),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!ok) {
        SDL_Log("CreateProcess failed");
        exit(1);
    }

    // Parent should close child side of pipes
    CloseHandle(hChildStdoutWr);
    CloseHandle(hChildStdinRd);

    // Save handles for reading/writing
    hChildStdoutRd = hChildStdoutRd;
    hChildStdinWr  = hChildStdinWr;
#endif
}

void KataGoEngine::readerLoop() {
    char buffer[8192];
    std::string current;

#ifndef WINDOWS
    while(running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(outReadFd, &readfds);

        timeval tv{0, 200};

        int ret = select(outReadFd + 1, &readfds, nullptr, nullptr, &tv);
        if(ret > 0 && FD_ISSET(outReadFd, &readfds)) {
            ssize_t n = read(outReadFd, buffer, sizeof(buffer));
            if(n > 0) {
                current.append(buffer, n);
                size_t pos;
                while((pos = current.find('\n')) != std::string::npos) {
                    std::string line = current.substr(0, pos);
                    current.erase(0, pos + 1);

                    try {
                        json j = json::parse(line);
                        {
                            std::lock_guard<std::mutex> lk(qMutex);
                            messageQueue.push(j);
                        }
                        qCv.notify_one();
                    }
                    catch(...) {
                        // skip bad json
                    }
                }
            }
        }
    }
#else
    while (running) {
        DWORD available = 0;
        if (!PeekNamedPipe(hChildStdoutRd, NULL, 0, NULL, &available, NULL)) {
            Sleep(1);
            continue;
        }
        if (available == 0) {
            Sleep(1);
            continue;
        }

        DWORD bytesRead = 0;
        if (!ReadFile(hChildStdoutRd, buffer, sizeof(buffer), &bytesRead, NULL)) {
            Sleep(1);
            continue;
        }

        current.append(buffer, bytesRead);

        size_t pos;
        while ((pos = current.find('\n')) != std::string::npos) {
            std::string line = current.substr(0, pos);
            current.erase(0, pos + 1);

            try {
                json j = json::parse(line);
                {
                    std::lock_guard<std::mutex> lk(qMutex);
                    messageQueue.push(j);
                }
                qCv.notify_one();
            } catch (...) {
                // ignore invalid JSON
            }
        }
    }
#endif
}

void KataGoEngine::sendJSON(const json& j) {
    if (is_init_failure)
        return;

    std::string s = j.dump() + "\n";
#ifndef WINDOWS
    write(inWriteFd, s.c_str(), s.size());
#else
    DWORD written = 0;
    WriteFile(hChildStdinWr, s.c_str(), s.size(), &written, NULL);
#endif
}

json KataGoEngine::getJSON() {
    std::unique_lock<std::mutex> lk(qMutex);
    qCv.wait(lk, [&]{ return !messageQueue.empty(); });
    json j = messageQueue.front();
    messageQueue.pop();
    return j;
}

std::optional<std::vector<std::string>>
KataGoEngine::getNextMove(int topN) {
    if (is_init_failure)
        return std::nullopt;

    json msg = getJSON();

    if (!msg.contains("moveInfos") || msg["moveInfos"].empty()
            || !msg.contains("rootInfo") || !msg["rootInfo"].contains("currentPlayer")
    ) {
        GoErrorHandler::throwError(GoErrorEnum::ENGINE_NOT_USABLE);
        return std::nullopt;
    }

    std::vector<std::string> move(2);
    move[0] = msg["rootInfo"]["currentPlayer"];
    move[1] = msg["moveInfos"][0]["move"];

    return std::make_optional<std::vector<std::string>>(move);
}

std::optional<KataGoEvaluation>
KataGoEngine::getEvaluation () {
    if (is_init_failure)
        return std::nullopt;

    json msg = getJSON();

    if (!msg.contains("ownership") || !msg.contains("rootInfo") || !msg["rootInfo"].contains("scoreLead")) {
        GoErrorHandler::throwError(GoErrorEnum::ENGINE_NOT_USABLE);
        return std::nullopt;
    }

    int board_size = std::sqrt(msg["ownership"].size());
    std::vector<std::vector<double>> ownership(board_size, std::vector<double>(board_size, 0));
    for (int i = 0; i < msg["ownership"].size(); i++) {
        int x = i/board_size;
        int y = i%board_size;
        ownership[x][y] = msg["ownership"][i];
    }

    KataGoEvaluation eval;

    return std::make_optional<KataGoEvaluation>({msg["rootInfo"]["scoreLead"], ownership});
}
