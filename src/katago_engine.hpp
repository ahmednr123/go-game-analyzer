#ifndef KATAGO_ENGINE_HPP
#define KATAGO_ENGINE_HPP

#define _DARWIN_C_SOURCE
#include <signal.h>

#include <functional>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <unistd.h>
#include <fcntl.h>
#include "json.hpp"

#ifndef WINDOWS
#include <sys/select.h>
#include <sys/wait.h>
#else
#include <windows.h>
#endif

struct KataGoEvaluation {
    double score;
    std::vector<std::vector<double>> ownership;
};

class KataGoEngine {
private:

#ifndef WINDOWS
    int inWriteFd = -1;
    int outReadFd = -1;
    pid_t childPid = -1;
#else
    HANDLE hChildStdinWr = NULL;
    HANDLE hChildStdoutRd = NULL;
    PROCESS_INFORMATION pi{};
#endif

    bool is_init_failure = false;
    std::thread readerThread;
    std::atomic<bool> running{true};

    std::mutex qMutex;
    std::condition_variable qCv;
    std::queue<nlohmann::json> messageQueue;

    void readerLoop();
    void startProcess(
        const std::string& katagoPath,
        const std::string& configPath,
        const std::string& modelPath
    );

    nlohmann::json getJSON();

public:
    KataGoEngine(
        const std::string& katagoPath,
        const std::string& configPath,
        const std::string& modelPath,
        std::function<void(bool)> init_callback
    );

    ~KataGoEngine();

    void sendJSON(const nlohmann::json& j);

    std::optional<std::vector<std::string>>
        getNextMove (int topN = 1);

    std::optional<KataGoEvaluation>
        getEvaluation ();
};

#endif
