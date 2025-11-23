#ifndef GO_DRAW_H
#define GO_DRAW_H

#include "base.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <string>

class GoDrawHelper {
public:
    static void DrawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius);
    static void DrawOutlinedCircle(SDL_Renderer* renderer, int cx, int cy, int radius);
    static void DrawStone (SDL_Renderer* renderer, GoBoardInfo board, GoStone stone, int alpha);
    static void DrawStone (SDL_Renderer* renderer, GoBoardInfo board, GoStone stone);
    static void DrawStraightLine (SDL_Renderer* renderer, float ax, float ay, float bx, float by, int stroke);
    static void DrawStraightLine (SDL_Renderer* renderer, float ax, float ay, float bx, float by);
    static void DrawBoard (SDL_Renderer* renderer, GoBoardInfo board);
    static void DrawText (TTF_TextEngine* text_engine, TTF_Font* font, std::pair<int, int> point, std::string str, int font_size, GoTextAlign align = GoTextAlign::LEFT_ALIGN);

    // Prevent instantiation
    GoDrawHelper() = delete;
    GoDrawHelper(const GoDrawHelper&) = delete;
    GoDrawHelper& operator=(const GoDrawHelper&) = delete;
};

#endif
