#include "rules.hpp"
#include "base.hpp"

GoBoardRuleManager::GroupResult
GoBoardRuleManager::exploreGroupAndLiberties(
    std::vector<std::vector<bool>>& visited,
    const GoBoardStateComputed& board,
    GoTurn target, int sx, int sy
) {
    GroupResult result;
    std::queue<std::pair<int,int>> q;

//    GoBoardCellState startState = board.get(sx, sy);
//    auto startTurn = getTurnFromCellState(startState);
//    if (!startTurn.has_value()) return result; // empty cell, not a group
//
//    GoTurn target = startTurn.value();
    q.push({sx, sy});
    visited[sx][sy] = true;

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        result.stones.push_back({target, x, y});

        for (auto [dx, dy] : DIRECTIONS) {
            int nx = x + dx, ny = y + dy;

            if (nx < 0 || ny < 0 || nx >= board.getSize() || ny >= board.getSize())
                continue;

            if (visited[nx][ny]) continue;

            GoBoardCellState cell = board.get(nx, ny);
            auto turn = getTurnFromCellState(cell);

            if (!turn.has_value()) {
                result.has_liberty = true;  // found a liberty
                continue;
            }

            if (turn.value() == target) {
                visited[nx][ny] = true;
                q.push({nx, ny});
            }
        }
    }

    return result;
}

std::vector<std::vector<GoStone>>
GoBoardRuleManager::getCapturedGroups(const GoBoardStateComputed& board, GoStone placedStone)
{
    int N = board.getSize();
    std::vector<std::vector<bool>> visited(N, std::vector<bool>(N, false));
    std::vector<std::vector<GoStone>> captured;

    visited[placedStone.x][placedStone.y] = true;

    for (auto [dx, dy] : DIRECTIONS) {
        int nx = placedStone.x + dx;
        int ny = placedStone.y + dy;

        if (nx < 0 || ny < 0 || nx >= N || ny >= N) continue;
        if (visited[nx][ny]) continue;

        GoBoardCellState cell = board.get(nx, ny);
        auto turn = getTurnFromCellState(cell);
        if (!turn.has_value()) continue;

        bool is_enemy = turn.value() != placedStone.turn;
        if (!is_enemy) continue;

        auto group =
            exploreGroupAndLiberties(
                visited, board,
                placedStone.turn == GoTurn::WHITE ?
                    GoTurn::BLACK :
                    GoTurn::WHITE,
                nx, ny
            );
        if (!group.has_liberty) {
            captured.push_back(group.stones);
        }
    }

    return captured;
}

bool GoBoardRuleManager::isValidStoneIgnoringCapture(const GoBoardStateComputed& board, GoStone stone)
{
    int x = stone.x, y = stone.y;
    int N = board.getSize();
    std::vector<std::vector<bool>> visited(N, std::vector<bool>(N, false));

    // Check immediate liberties
    for (auto [dx, dy] : DIRECTIONS) {
        int nx = x + dx, ny = y + dy;

        if (nx < 0 || ny < 0 || nx >= N || ny >= N) continue;

        GoBoardCellState cell = board.get(nx, ny);
        auto turn = getTurnFromCellState(cell);

        if (!turn.has_value())
            return true; // direct liberty

    }

    // Check if our own group has liberties
    auto group = exploreGroupAndLiberties(visited, board, stone.turn, x, y);
    if (group.has_liberty)
        return true;

    return false;
}

bool GoBoardRuleManager::isValidStone(const GoBoardStateComputed& board, GoStone stone)
{
    if (!isValidStoneIgnoringCapture(board, stone)) {
        // Check if we capture something (ko/suicide rule)
        auto captured = getCapturedGroups(board, stone);
        return !captured.empty();
    }

    return true;
}
