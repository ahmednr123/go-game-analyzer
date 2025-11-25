#ifndef GO_RULES_H
#define GO_RULES_H

#include "base.hpp"
#include "state.hpp"
#include <optional>
#include <queue>

#define DIRECTION_UP 0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3

const std::vector<std::pair<int, int>> DIRECTIONS = {{0,-1}, {1,0}, {0,1}, {-1,0}};

class GoBoardRuleManager {
private:
    struct GroupResult {
        std::vector<GoStone> stones;  // all stones in the group
        bool has_liberty = false;     // at least one adjacent empty
    };

    static GroupResult exploreGroupAndLiberties(
        std::vector<std::vector<bool>>& visited,
        const GoBoardStateComputed& board,
        GoTurn target, int sx, int sy
    );

public:
    static std::vector<std::vector<GoStone>>
    getCapturedGroups(const GoBoardStateComputed& board, GoStone placedStone);

    static bool isValidStoneIgnoringCapture(const GoBoardStateComputed& board, GoStone stone);
    static bool isValidStone(const GoBoardStateComputed& board, GoStone stone);

    // Prevent instantiation
    GoBoardRuleManager() = delete;
    GoBoardRuleManager(const GoBoardRuleManager&) = delete;
    GoBoardRuleManager& operator=(const GoBoardRuleManager&) = delete;
};

#endif
