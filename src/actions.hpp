#ifndef GO_ACTIONS_H
#define GO_ACTIONS_H

#include "base.hpp"
#include <variant>
#include <vector>

//enum class GoBoardActionType {
//    ADD_STONE,
//    CAPTURE_STONES,
//    PASS
//};

struct PassAction {
    GoTurn turn;
};

struct AddStoneAction {
    GoStone stone;
};

struct CaptureStonesAction {
    GoStone capturing_stone;
    std::vector<GoStone> removed_stones;
};

struct NoAction {};

using GoBoardAction = std::variant<NoAction, CaptureStonesAction, AddStoneAction, PassAction>;

#endif
