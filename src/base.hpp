#ifndef GO_BASE_H
#define GO_BASE_H

#include <SDL3/SDL_render.h>
#include <string>
#include <unordered_map>
#include <vector>

enum class GoBoardSize: int {
    _9x9    = 9,
    _13x13  = 13,
    _19x19  = 19
};

const std::unordered_map<GoBoardSize, std::vector<std::vector<int>>> BOARD_DOTS = {
    {GoBoardSize::_9x9,  {{2,2}, {6,2}, { 4,4}, {2,6}, {6,6}}},
    {GoBoardSize::_13x13, {{3,3}, {9,3}, { 6,6}, {3,9}, {9,9}}},
    {GoBoardSize::_19x19, {{3, 3}, { 9,3}, {15, 3},
          {3, 9}, { 9,9}, {15, 9},
          {3,15}, {9,15}, {15,15}}
    }
};

const SDL_Color OFF_WHITE_COLOR = {200, 200, 200, 255};
const SDL_Color RED_COLOR = {230, 67, 67, 255};

enum class GoTextAlign {
    LEFT_ALIGN,
    MIDDLE_ALIGN,
    RIGHT_ALIGN
};

enum class GoTurn {
    BLACK,
    WHITE
};

struct GoStone {
    GoTurn turn;
    int x;
    int y;

    bool operator<(const GoStone& other) const {
        if (x != other.x) {
            return x < other.x;
        }
        return y < other.y;
    }

    bool operator==(const GoStone& other) const {
        return turn == other.turn &&
               x == other.x &&
               y == other.y;
    }

    GoStone(GoTurn turn, int px, int py) : turn(turn), x(px), y(py) {}
};

struct GoBoardInfo {
    float size;
    float x, y;
    float inner_size;
    float inner_gap;
    float inner_x, inner_y;
    GoBoardSize dim;
};

inline GoBoardInfo GetGoBoardInfo (int w, int h, GoBoardSize size) {
    float board_size = (w > h) ? h - 80 : w - 80;

    float rect_x = (w/2.0) - (board_size/2.0);
    float rect_y = (h/2.0) - (board_size/2.0);

    float inner_gap = board_size / (static_cast<int>(size) + 1);

    float inner_size = board_size - inner_gap * 2;
    float inner_x = (w/2.0) - (inner_size/2.0);
    float inner_y = (h/2.0) - (inner_size/2.0);

    return {
        board_size,
        rect_x, rect_y,
        inner_size,
        inner_gap,
        inner_x, inner_y,
        size
    };
}

#endif
