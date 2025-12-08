#ifndef GO_UTILS_H
#define GO_UTILS_H

#include "SDL3/SDL_pixels.h"
#include "base.hpp"
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <chrono>

inline bool isPointInCircle(
    float px, float py,
    float cx, float cy,
    float radius
) {
    float dx = px - cx;
    float dy = py - cy;
    float dist2 = dx*dx + dy*dy;
    return dist2 <= radius * radius;
}

inline std::optional<std::pair<int, int>>
getBoardCellFromPoint (GoBoardInfo board, float px, float py) {
    int board_dim = static_cast<int>(board.dim);

    for (int x = 0; x < board_dim; x++) {
        for (int y = 0; y < board_dim; y++) {
            float cx = board.inner_x + board.inner_gap * x;
            float cy = board.inner_y + board.inner_gap * y;
            float r = board.inner_gap/2 - 5;

            if (isPointInCircle(px, py, cx, cy, r)) {
                return std::make_optional<std::pair<int,int>>(x, y);
            }
        }
    }

    return std::nullopt;
}

inline  std::string
genRandomString (int length) {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string randomString;
    randomString.reserve(length);

    for (int i = 0; i < length; ++i) {
        randomString += CHARACTERS[distribution(generator)];
    }

    return randomString;
}

inline long
getCurrentMillis () {
    auto now = std::chrono::system_clock::now();
    auto duration_since_epoch = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration_since_epoch);
    return milliseconds.count();
}

inline std::optional<SDL_Color>
hexToSDLColor(const std::string& hexCode) {
    SDL_Color color;
    color.a = 1.0; // Default alpha to 1.0 (fully opaque)

    std::string cleanHex = hexCode;
    if (cleanHex[0] == '#') {
        cleanHex = cleanHex.substr(1); // Remove '#' prefix
    }

    // #RRGGBB format
    if (cleanHex.length() == 6) {
        color.r = std::stoi(cleanHex.substr(0, 2), nullptr, 16);
        color.g = std::stoi(cleanHex.substr(2, 2), nullptr, 16);
        color.b = std::stoi(cleanHex.substr(4, 2), nullptr, 16);
    }

    // #RRGGBBAA format
    else if (cleanHex.length() == 8) {
        color.r = std::stoi(cleanHex.substr(0, 2), nullptr, 16);
        color.g = std::stoi(cleanHex.substr(2, 2), nullptr, 16);
        color.b = std::stoi(cleanHex.substr(4, 2), nullptr, 16);
        int alphaHex = std::stoi(cleanHex.substr(6, 2), nullptr, 16);
        color.a = static_cast<double>(alphaHex) / 255.0;
    }

    // Invalid format
    else {
        std::cerr << "Invalid hex code length: " << hexCode << std::endl;
        return std::nullopt;
    }

    return std::make_optional<SDL_Color>(color);
}

#endif
