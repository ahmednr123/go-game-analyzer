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

/*
//15-20 kyu
  "maxVisits": 20,
  "rootPolicyTemperature": 1.4,
  "rootFpuReductionMax": 0.0,
  "rootNoiseEnabled": true,
  "rootDirichletAlpha": 0.6

//10-15kyu
  "maxVisits": 60,
  "rootPolicyTemperature": 0.9,
  "rootFpuReductionMax": 0.1,
  "rootNoiseEnabled": true,
  "rootDirichletAlpha": 0.4

//5-10kyu
  "id": "foo",
  "maxVisits": 150,
  "rootPolicyTemperature": 0.6,
  "rootFpuReductionMax": 0.25,
  "rootNoiseEnabled": false

//1-2kyu
  "id": "foo",
  "maxVisits": 400,
  "rootPolicyTemperature": 0.45,
  "rootFpuReductionMax": 0.3,
  "rootNoiseEnabled": false

//strong!
  "id": "foo",
  "maxVisits": 2000,
  "rootPolicyTemperature": 0.3,
  "rootFpuReductionMax": 0.5,
  "rootNoiseEnabled": false
*/


class KataGoEngine {
public:
    KataGoEngine(const std::string& katagoPath,
                 const std::string& configPath,
                 const std::string& modelPath);

    ~KataGoEngine();

    void sendJSON(const nlohmann::json& j);

    nlohmann::json getJSON();

    std::vector<std::string> getNextMove (int topN = 1);
    std::vector<std::vector<double>> getOwnership ();

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
};

#endif
