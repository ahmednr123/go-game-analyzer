#ifndef GO_ERROR_H
#define GO_ERROR_H

#include "SDL3/SDL_log.h"
#include "utils.hpp"
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#define WARNING_SHOW_MILLIS 2000L

enum class GoErrorSeverity {
    DEBUG,
    UNRECOVERABLE,
    RECOVERABLE,
    WARNING
};

enum class GoErrorEnum {
    BOARD_STATE_CORRUPT,
    UNPARSEABLE_MOVE,
    ENGINE_NOT_FOUND,
    ENGINE_VERSION_MISMATCH,
    ENGINE_PARSE_ERROR,
    ENGINE_CONFIG_FILE_NOT_FOUND,
    ENGINE_MODEL_FILE_NOT_FOUND,
    ENGINE_BUSY,
    ENGINE_NOT_USABLE,
    GAME_IN_KO
};

struct GoError {
    GoErrorSeverity severity;
    std::string message;
};

const std::unordered_map<GoErrorEnum, GoError> GO_ERRORS = {
    {GoErrorEnum::BOARD_STATE_CORRUPT, {GoErrorSeverity::RECOVERABLE, "Board state error due to invalid action added to the board actions"}},
    {GoErrorEnum::UNPARSEABLE_MOVE, {GoErrorSeverity::DEBUG, "Move parsed cannot be handled, handle this situation manually"}},
    {GoErrorEnum::ENGINE_NOT_FOUND, {GoErrorSeverity::WARNING, "Engine not connected, please update path in config.json and restart"}},
    {GoErrorEnum::ENGINE_VERSION_MISMATCH, {GoErrorSeverity::WARNING, "Engine version mismatch, issues may occur. Use version: "}},
    {GoErrorEnum::ENGINE_PARSE_ERROR, {GoErrorSeverity::DEBUG, "Response from katago cannot be parsed properly."}},
    {GoErrorEnum::ENGINE_CONFIG_FILE_NOT_FOUND, {GoErrorSeverity::WARNING, "Engine config file not found, Engine failed to load"}},
    {GoErrorEnum::ENGINE_MODEL_FILE_NOT_FOUND, {GoErrorSeverity::WARNING, "Engine model file not found, Engine failed to load"}},
    {GoErrorEnum::ENGINE_BUSY, {GoErrorSeverity::WARNING, "Engine is busy"}},
    {GoErrorEnum::ENGINE_NOT_USABLE, {GoErrorSeverity::WARNING, "Engine parse error, disabling it for now."}},
    {GoErrorEnum::GAME_IN_KO, {GoErrorSeverity::WARNING, "The game is in KO, play elsewhere first."}}
};

struct GoErrorPacket {
    long _at;
    GoError error;
};

class GoErrorHandler {
    static std::vector<GoErrorPacket> error_packets;

public:
    static void init () {
        error_packets = {};
    }

    static void throwError(GoErrorEnum error) {
        error_packets.push_back({getCurrentMillis(), GO_ERRORS.at(error)});
    }

    static std::optional<GoErrorSeverity> getErrorSeverity (const std::vector<GoError>& errors) {
        std::optional<GoErrorSeverity> severity = std::nullopt;
        for (GoError error : errors) {
            if (!severity.has_value()) {
                severity = std::make_optional<GoErrorSeverity>(error.severity);
            }

            if (error.severity < severity.value()) {
                severity = std::make_optional<GoErrorSeverity>(error.severity);
            }
        }
        return severity;
    }

    static std::vector<GoError> getErrors() {
        if (error_packets.size() == 0)
            return {};

        long current_millis = getCurrentMillis();
        std::vector<GoError> errors;
        for (auto it = error_packets.begin(); it != error_packets.end();) {
            auto packet = *it;

            if (packet.error.severity == GoErrorSeverity::WARNING) {
                if (current_millis > packet._at + WARNING_SHOW_MILLIS) {
                    error_packets.erase(it);
                    continue;
                }
            }

            errors.push_back(packet.error);
            it++;
        }

        return errors;
    }

    // Prevent instantiation
    GoErrorHandler() = delete;
    GoErrorHandler(const GoErrorHandler&) = delete;
    GoErrorHandler& operator=(const GoErrorHandler&) = delete;
};

#endif
