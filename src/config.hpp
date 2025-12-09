#ifndef GO_CONFIG_H
#define GO_CONFIG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "json.hpp"
#include "utils.hpp"

class GoGameConfig {
    static bool engine_enabled;
    static std::string katago_path;
    static std::string katago_config_path;
    static std::string model_path;

public:
    static void init (std::string config_path) {
        std::ifstream input_file(config_path);

        if (!input_file.is_open()) {
            std::cerr << "Error opening the config file, loading default config" << std::endl;
            GoGameConfig::engine_enabled        = false;
#ifndef WINDOWS
            GoGameConfig::katago_path           = "katago";
#else
            GoGameConfig::katago_path           = "katago.exe";
#endif
            GoGameConfig::katago_config_path    = "./assets/KataGo/config/analysis_example.cfg";
            GoGameConfig::model_path            = "./assets/KataGo/models/kata1-b18c384nbt-s9996604416-d4316597426.bin.gz";

            return;
        }

        std::stringstream buffer;
        buffer << input_file.rdbuf();
        std::string file_content = buffer.str();

        nlohmann::json parsed_json          = nlohmann::json::parse(file_content);
        GoGameConfig::engine_enabled        = getJSONOrDefault(parsed_json, "engine_enabled", true);
#ifndef WINDOWS
        GoGameConfig::katago_path           = getJSONOrDefault(parsed_json, "katago_path", "katago");
#else
        GoGameConfig::katago_path           = getJSONOrDefault(parsed_json, "katago_path", "katago.exe");
#endif
        GoGameConfig::katago_config_path    = getJSONOrDefault(parsed_json, "config_path", "./assets/KataGo/config/analysis_example.cfg");
        GoGameConfig::model_path            = getJSONOrDefault(parsed_json, "model_path", "./assets/KataGo/models/kata1-b18c384nbt-s9996604416-d4316597426.bin.gz");

        input_file.close();
    }

    static bool isEngineEnabled () { return engine_enabled; }
    static std::string getKatagoPath () { return katago_path; }
    static std::string getKatagoConfigPath () { return katago_config_path; }
    static std::string getModelPath () { return model_path; }
};

#endif
