#ifndef GO_KATAGO_SETTINGS_H
#define GO_KATAGO_SETTINGS_H

#include "json.hpp"

using nlohmann::json;

inline void addDiffLevelToKatagoRequest (json& req, int diff_lvl) {
    switch (diff_lvl) {
        case 4:
            //10-15kyu
            req["maxVisits"] = 60;
            req["rootPolicyTemperature"] = 0.9;
            req["rootFpuReductionMax"] = 0.1;
            break;
        case 3:
            //5-10kyu
            req["maxVisits"] = 150;
            req["rootPolicyTemperature"] = 0.6;
            req["rootFpuReductionMax"] = 0.25;
            break;
        case 2:
            //1-2kyu
            req["maxVisits"] = 400;
            req["rootPolicyTemperature"] = 0.45;
            req["rootFpuReductionMax"] = 0.3;
            break;
        case 1:
            //Strongest (A lower value to reduce response time)
            req["maxVisits"] = 800;
            req["rootPolicyTemperature"] = 0.3;
            req["rootFpuReductionMax"] = 0.5;
            break;
        default:
        case 5:
            //15-20 kyu
            req["maxVisits"] = 20;
            req["rootPolicyTemperature"] = 1.4;
            req["rootFpuReductionMax"] = 0.0;
            break;
    }
}

#endif
