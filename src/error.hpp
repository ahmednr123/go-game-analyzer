#ifndef GO_ERROR_H
#define GO_ERROR_H

#include <string>
#include <unordered_map>

#define GO_SEVERITY_HIGH 0
#define GO_SEVERITY_LOW 1

enum class GoErrorEnum {
    BOARD_STATE_CORRUPT
};

struct GoError {
    int severity;
    std::string message;
};

const std::unordered_map<GoErrorEnum, GoError> GO_ERRORS = {
    {GoErrorEnum::BOARD_STATE_CORRUPT, {GO_SEVERITY_HIGH, "Board state error due to invalid action added to the board actions"}}
};


#endif
