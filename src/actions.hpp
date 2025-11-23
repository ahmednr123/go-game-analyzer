#ifndef GO_ACTIONS_H
#define GO_ACTIONS_H

#include "base.hpp"
#include <variant>
#include <vector>

enum class GoBoardActionType {
    ADD_STONE,
    REMOVE_STONES
};

struct AddStone {
    GoStone stone;
};

struct RemoveStones {
    std::vector<GoStone> stones;
};

using GoBoardAction = std::variant<RemoveStones, AddStone>;

#endif
