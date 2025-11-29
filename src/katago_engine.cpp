#include "katago_engine.hpp"
#include <SDL3/SDL_log.h>
#include <cmath>
#include <iostream>

using json = nlohmann::json;

static void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

KataGoEngine::KataGoEngine(const std::string& katagoPath,
                           const std::string& configPath,
                           const std::string& modelPath) {
    startProcess(katagoPath, configPath, modelPath);
    readerThread = std::thread(&KataGoEngine::readerLoop, this);
}

KataGoEngine::~KataGoEngine() {
    running = false;

    if(readerThread.joinable())
        readerThread.join();

    if(childPid > 0) {
        kill(childPid, SIGTERM);
        waitpid(childPid, nullptr, 0);
    }
}

void KataGoEngine::startProcess(const std::string& katagoPath,
                                const std::string& configPath,
                                const std::string& modelPath) {
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
}

void KataGoEngine::readerLoop() {
    char buffer[8192];
    std::string current;

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
}

void KataGoEngine::sendJSON(const json& j) {
    std::string s = j.dump() + "\n";
    write(inWriteFd, s.c_str(), s.size());
}

json KataGoEngine::getJSON() {
    std::unique_lock<std::mutex> lk(qMutex);
    qCv.wait(lk, [&]{ return !messageQueue.empty(); });
    json j = messageQueue.front();
    messageQueue.pop();
    return j;
}

std::vector<std::string> KataGoEngine::getNextMove(int topN) {
    json msg = getJSON();

    if (!msg.contains("moveInfos") || msg["moveInfos"].empty()
            || !msg.contains("rootInfo") || !msg["rootInfo"].contains("currentPlayer")
    ) {
        // Bad json
    }

    return {
        msg["rootInfo"]["currentPlayer"],
        msg["moveInfos"][0]["move"]
    };
}

KataGoEvaluation KataGoEngine::getEvaluation () {
    json msg = getJSON();

    if (!msg.contains("ownership") || !msg.contains("rootInfo") || !msg["rootInfo"].contains("scoreLead")) {
        // Bad json
        SDL_Log("getEvaluation JSON ERROR!");
    }

    int board_size = std::sqrt(msg["ownership"].size());
    std::vector<std::vector<double>> ownership(board_size, std::vector<double>(board_size, 0));
    for (int i = 0; i < msg["ownership"].size(); i++) {
        int x = i/board_size;
        int y = i%board_size;
        ownership[x][y] = msg["ownership"][i];
    }

    return {msg["rootInfo"]["scoreLead"], ownership};
}
