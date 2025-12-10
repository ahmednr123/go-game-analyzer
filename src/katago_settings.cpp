#include "katago_settings.hpp"
#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

KataGoSetting getDefaultSetting (KataGoLevel level) {
    switch (level) {
        case KataGoLevel::LEVEL_4:
            //10-15kyu
            return {60, 0.9, 0.1};
            break;
        case KataGoLevel::LEVEL_3:
            //5-10kyu
            return {150, 0.6, 0.25};
        case KataGoLevel::LEVEL_2:
            //1-2kyu
            return {400, 0.45, 0.3};
        case KataGoLevel::LEVEL_1:
            //Strongest (A lower value to reduce response time)
            return {800, 0.3, 0.5};
        default:
        case KataGoLevel::LEVEL_5:
            //15-20 kyu
            return {20, 1.4, 0.0};
    }
}

KataGoSetting KataGoSettings::getSetting (KataGoLevel level) {
    //try to read from file, if not found fallback to defaults
    std::ifstream input_file("./assets/KataGo/config/settings.json");

    if (!input_file.is_open()) {
        std::cerr << "Error opening the katago settings file, loading default settings" << std::endl;
        return getDefaultSetting(level);
    }

    try {
        std::stringstream buffer;
        buffer << input_file.rdbuf();
        std::string file_content = buffer.str();
        input_file.close();

        nlohmann::json parsed_json = nlohmann::json::parse(file_content);
        if (level == KataGoLevel::EVAL) {
            KataGoSetting setting;
            setting.max_visits = getJSONOrDefault(parsed_json["evaluation"], "maxVisits", 20);
            setting.temprature = getJSONOrDefault(parsed_json["evaluation"], "rootPolicyTemperature", 1.4);
            setting.fpu_red_max = getJSONOrDefault(parsed_json["evaluation"], "rootFpuReductionMax", 0.0);
            return setting;
        }

        std::string level_str = getLevelString(level);
        if (!parsed_json.contains("levels")
                || !parsed_json["levels"].contains(level_str))
        {
            throw std::runtime_error("Json error");
        }

        KataGoSetting setting;
        setting.max_visits = getJSONOrDefault(parsed_json["levels"][level_str], "maxVisits", 20);
        setting.temprature = getJSONOrDefault(parsed_json["levels"][level_str], "rootPolicyTemperature", 1.4);
        setting.fpu_red_max = getJSONOrDefault(parsed_json["levels"][level_str], "rootFpuReductionMax", 0.0);
        return setting;
    } catch (...) {
        std::cerr << "Error parsing the katago settings file, loading default settings" << std::endl;
    }

    return getDefaultSetting(level);
}

void KataGoSettings::applyDiffLevel (json& req, KataGoLevel level) {
    KataGoSetting setting = getSetting(level);
    req["maxVisits"] = setting.max_visits;
    req["rootPolicyTemperature"] = setting.temprature;
    req["rootFpuReductionMax"] = setting.fpu_red_max;
}

void KataGoSettings::applyEvaluationConfig (json& req) {
    KataGoSetting setting = getSetting(KataGoLevel::EVAL);
    req["maxVisits"] = setting.max_visits;
    req["rootPolicyTemperature"] = setting.temprature;
    req["rootFpuReductionMax"] = setting.fpu_red_max;
}

