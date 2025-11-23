#ifndef GO_UTILS_H
#define GO_UTILS_H

#include "base.hpp"
#include <optional>
#include <utility>
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
                return std::make_optional<std::pair<int, int>>({x, y});
            }
        }
    }

    return std::nullopt;
}

#endif
