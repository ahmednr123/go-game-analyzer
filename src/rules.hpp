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

//class GoBoardRuleManager {
//private:
//    static std::vector<GoStone>
//    _getCapturedGroup (
//        const GoBoardStateComputed& computed,
//        std::vector<std::vector<bool>>& visited_map,
//        std::vector<GoStone>& stones,
//        GoStone stone
//    ) {
//        visited_map[stone.x][stone.y] = true;
//        for (int dir = 0; dir < DIRECTIONS.size(); dir++) {
//            int _x = stone.x + DIRECTIONS[dir].first;
//            int _y = stone.y + DIRECTIONS[dir].second;
//
//            if (_x < 0 || _x >= computed.getSize()
//                    || _y < 0 || _y >= computed.getSize())
//            {
//                continue;
//            }
//
//            if (visited_map[_x][_y]) continue;
//            visited_map[_x][_y] = true;
//
//            GoBoardCellState cell_state = computed.get(_x, _y);
//            std::optional<GoTurn> turn_opt = getTurnFromCellState(cell_state);
//            if (!turn_opt.has_value()) {
//                return {};
//            }
//
//            if (isCellTurn(cell_state, stone)) {
//                return _getCapturedGroup(
//                        computed, visited_map, stones,
//                        {turn_opt.value(), _x, _y}
//                    );
//            }
//        }
//
//        return stones;
//    }
//
//public:
//    static std::vector<std::vector<GoStone>>
//    getCapturedGroups (
//        const GoBoardStateComputed& computed,
//        GoStone stone
//    ) {
//        std::vector<std::vector<bool>>
//            visited_map(
//                computed.getSize(),
//                std::vector<bool>(computed.getSize(), false)
//            );
//        std::vector<std::vector<GoStone>> stone_groups;
//
//        visited_map[stone.x][stone.y] = true;
//        for (int dir = 0; dir < DIRECTIONS.size(); dir++) {
//            int _x = stone.x + DIRECTIONS[dir].first;
//            int _y = stone.y + DIRECTIONS[dir].second;
//
//            if (_x < 0 || _x >= computed.getSize()
//                    || _y < 0 || _y >= computed.getSize())
//            {
//                continue;
//            }
//
//            if (visited_map[_x][_y]) continue;
//            visited_map[_x][_y] = true;
//
//            GoBoardCellState cell_state = computed.get(_x, _y);
//            std::optional<GoTurn> turn_opt = getTurnFromCellState(cell_state);
//            if (!turn_opt.has_value()) {
//                continue;
//            }
//
//            bool is_other_stone =
//                (stone.turn == GoTurn::BLACK && turn_opt.value() == GoTurn::WHITE)
//                || (stone.turn == GoTurn::WHITE && turn_opt.value() == GoTurn::BLACK);
//
//            if (is_other_stone) {
//                std::vector<GoStone> stones = _getCapturedGroup(
//                        computed, visited_map, stones,
//                        {turn_opt.value(), _x, _y}
//                    );
//                if (stones.size() > 0)
//                    stone_groups.push_back(stones);
//            }
//        }
//
//        return stone_groups;
//    }
//
//    static bool isValidStone (const GoBoardStateComputed& computed, GoStone stone) {
//        int x = stone.x;
//        int y = stone.y;
//
//        std::vector<std::vector<bool>>
//            visited_map(
//                computed.getSize(),
//                std::vector<bool>(computed.getSize(), false)
//            );
//
//        visited_map[x][y] = true;
//        for (int dir = 0; dir < DIRECTIONS.size(); dir++) {
//            int _x = x + DIRECTIONS[dir].first;
//            int _y = y + DIRECTIONS[dir].second;
//
//            if (_x < 0 || _x >= computed.getSize()
//                    || _y < 0 || _y >= computed.getSize())
//            {
//                continue;
//            }
//
//            if (visited_map[_x][_y]) continue;
//            visited_map[_x][_y] = true;
//
//            GoBoardCellState cell_state = computed.get(_x, _y);
//            std::optional<GoTurn> turn_opt = getTurnFromCellState(cell_state);
//            if (!turn_opt.has_value()) {
//                return true;
//            }
//
//            // Valid if the group has liberties
//            bool is_same_stone =
//                (stone.turn == GoTurn::BLACK && turn_opt.value() == GoTurn::BLACK)
//                || (stone.turn == GoTurn::WHITE && turn_opt.value() == GoTurn::WHITE);
//
//            if (is_same_stone) {
//                std::vector<GoStone> stones = _getCapturedGroup(
//                        computed, visited_map, stones,
//                        {turn_opt.value(), _x, _y}
//                    );
//                // If no captured group is found that means liberties exist
//                if (stones.size() == 0) {
//                    return true;
//                }
//            }
//        }
//
//        return false;
//    }
//};

#endif
