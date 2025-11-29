#ifndef KATAGO_ENGINE_HPP
#define KATAGO_ENGINE_HPP

#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "json.hpp"

struct KataGoEvaluation {
    double score;
    std::vector<std::vector<double>> ownership;
};

class KataGoEngine {
private:
    int inWriteFd = -1;
    int outReadFd = -1;
    pid_t childPid = -1;

    std::thread readerThread;
    std::atomic<bool> running{true};

    std::mutex qMutex;
    std::condition_variable qCv;
    std::queue<nlohmann::json> messageQueue;

    void readerLoop();
    void startProcess(const std::string&, const std::string&, const std::string&);

public:
    KataGoEngine(const std::string& katagoPath,
                 const std::string& configPath,
                 const std::string& modelPath);

    ~KataGoEngine();

    void sendJSON(const nlohmann::json& j);

    nlohmann::json getJSON();

    std::vector<std::string> getNextMove (int topN = 1);
    KataGoEvaluation getEvaluation ();

};

#endif
