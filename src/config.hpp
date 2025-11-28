#ifndef GO_CONFIG_H
#define GO_CONFIG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "json.hpp"

class GoGameConfig {
    static std::string katago_path;
    static std::string katago_config_path;
    static std::string model_path;

public:
    static void init (std::string config_path) {
        std::ifstream input_file(config_path);

        if (!input_file.is_open()) {
            std::cerr << "Error opening the config file, loading default config" << std::endl;
            GoGameConfig::katago_path           = "./katago";
            GoGameConfig::katago_config_path    = "./analysis_example.cfg";
            GoGameConfig::model_path            = "./g170e-b20c256x2-s5303129600-d1228401921.bin.gz";

            return;
        }

        std::stringstream buffer;
        buffer << input_file.rdbuf();
        std::string file_content = buffer.str();

        nlohmann::json parsed_json = nlohmann::json::parse(file_content);
        GoGameConfig::katago_path = parsed_json["katago_path"];
        GoGameConfig::katago_config_path = parsed_json["config_path"];
        GoGameConfig::model_path = parsed_json["model_path"];

        input_file.close();
    }

    static std::string getKatagoPath () { return katago_path; }
    static std::string getKatagoConfigPath () { return katago_config_path; }
    static std::string getModelPath () { return model_path; }
};

#endif
