#include "theme.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include "json.hpp"
#include "utils.hpp"

int GoThemeHandler::current_idx = 0;
std::vector<GoTheme> GoThemeHandler::themes = {};

SDL_Color parseColorOrDefault (nlohmann::json parsed_json, std::string key, SDL_Color default_color) {
    if (parsed_json.contains(key)) {
        std::optional<SDL_Color> color_opt = hexToSDLColor(parsed_json[key]);
        if (color_opt.has_value())
            return color_opt.value();
    }
    return default_color;
}

std::optional<GoTheme> getThemeFromFile (std::string path) {
    std::ifstream input_file(path);

    if (!input_file.is_open()) {
        std::cerr << "Error opening the config file, loading default config" << std::endl;
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string file_content = buffer.str();

    nlohmann::json parsed_json = nlohmann::json::parse(file_content);
    input_file.close();

    GoTheme default_theme = GoThemeHandler::getDefaultTheme();

    SDL_Color bg_color = parseColorOrDefault(parsed_json, "background", default_theme.bg_color);
    SDL_Color board_color = parseColorOrDefault(parsed_json, "board", default_theme.board_color);
    SDL_Color board_line_color = parseColorOrDefault(parsed_json, "board_line", default_theme.board_line_color);
    SDL_Color black_color = parseColorOrDefault(parsed_json, "black_piece", default_theme.black_color);
    SDL_Color white_color = parseColorOrDefault(parsed_json, "white_piece", default_theme.white_color);
    SDL_Color text_color = parseColorOrDefault(parsed_json, "text", default_theme.text_color);
    SDL_Color error_text_color = parseColorOrDefault(parsed_json, "text_error", default_theme.error_text_color);

    bool hidden_board = false;
    if (parsed_json.contains("hidden_board"))
        hidden_board = parsed_json["hidden_board"];

    return std::make_optional<GoTheme>({
        bg_color,
        board_color,
        board_line_color,
        black_color,
        white_color,
        text_color,
        error_text_color,
        hidden_board
    });
}

void GoThemeHandler::init () {
    themes.push_back(getDefaultTheme());
}

bool GoThemeHandler::loadThemes () {
    namespace fs = std::filesystem;
    std::string themes_path = "assets/themes";
    if (!fs::exists(themes_path) || !fs::is_directory(themes_path)) {
        std::cerr << "Error: Directory '" << themes_path << "' does not exist or is not a directory." << std::endl;
        return false;
    }

    for (const auto& entry : fs::directory_iterator(themes_path)) {
        if (fs::is_regular_file(entry.status())) {
            std::string file_name = entry.path().filename().string();
            std::string file_path = themes_path + "/" + file_name;

            std::optional<GoTheme> theme_opt = getThemeFromFile(file_path);
            if (theme_opt.has_value()) {
                themes.push_back(theme_opt.value());
            }
        }
    }

    return true;
}

GoTheme GoThemeHandler::getTheme () {
    return themes[current_idx];
}

void GoThemeHandler::nextTheme () {
    current_idx = (current_idx + 1) % themes.size();
}

GoTheme GoThemeHandler::getDefaultTheme() {
    return {
        {0, 0, 0, 255}, // bg_color
        {186, 107, 58, 255}, // board_color
        {0, 0, 0, 255}, // board_line_color
        {50, 50, 50, 255}, // black_color
        {200, 200, 200, 255}, // white_color

        {200, 200, 200, 255}, // text_color
        {230, 67, 67, 255}, // error_text_color

        false
    };
}
