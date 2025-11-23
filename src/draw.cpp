#include "draw.hpp"
#include "base.hpp"

void GoDrawHelper::DrawFilledCircle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; dy++) {
        int dx = (int)std::sqrt(radius * radius - dy * dy);

        SDL_RenderLine(renderer,
                       cx - dx, cy + dy,
                       cx + dx, cy + dy);
    }
}

void GoDrawHelper::DrawOutlinedCircle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderPoint(renderer, cx + x, cy + y);
        SDL_RenderPoint(renderer, cx + y, cy + x);
        SDL_RenderPoint(renderer, cx - y, cy + x);
        SDL_RenderPoint(renderer, cx - x, cy + y);
        SDL_RenderPoint(renderer, cx - x, cy - y);
        SDL_RenderPoint(renderer, cx - y, cy - x);
        SDL_RenderPoint(renderer, cx + y, cy - x);
        SDL_RenderPoint(renderer, cx + x, cy - y);

        y += 1;

        if (err <= 0) {
            err += 2*y + 1;
        } else {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void GoDrawHelper::DrawStone (SDL_Renderer* renderer, GoBoardInfo board, GoStone stone, int alpha) {
    float x = board.inner_x + stone.x * board.inner_gap;
    float y = board.inner_y + stone.y * board.inner_gap;

    if (stone.turn == GoTurn::BLACK)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, alpha);
    else
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, alpha);

    DrawFilledCircle(renderer, x, y, board.inner_gap/2 - 5);
}

void GoDrawHelper::DrawStone (SDL_Renderer* renderer, GoBoardInfo board, GoStone stone) {
    DrawStone(renderer, board, stone, 255);
}

void GoDrawHelper::DrawStraightLine(SDL_Renderer *renderer, float ax, float ay, float bx, float by, int stroke) {
    if (stroke == 1) {
        SDL_RenderLine(renderer, ax, ay, bx, by);
        return;
    }

    float w = 0, h = 0;
    if (ax == bx) {
        w = stroke;
        h = by - ay;
    } else {
        w = bx - ax;
        h = stroke;
    }

    SDL_FRect rect = { ax, ay, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

void GoDrawHelper::DrawStraightLine(SDL_Renderer *renderer, float ax, float ay, float bx, float by) {
    DrawStraightLine(renderer, ax, ay, bx, by, 1);
}

void GoDrawHelper::DrawBoard(SDL_Renderer *renderer, GoBoardInfo board) {
    SDL_SetRenderDrawColor(renderer, 186, 107, 58, 255);
    SDL_FRect rect = { board.x, board.y, board.size, board.size };
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    int board_dim = static_cast<int>(board.dim);

    for (int i = 0; i <= board_dim; i++) {
        float dx = board.inner_x + (board.inner_gap * i);
        DrawStraightLine(renderer, dx, board.inner_y, dx, board.inner_y + board.inner_size);
    }

    for (int i = 0; i <= board_dim; i++) {
        float dy = board.inner_y + (board.inner_gap * i);
        DrawStraightLine(renderer, board.inner_x, dy, board.inner_x + board.inner_size, dy);
    }

    auto dots = BOARD_DOTS.at(board.dim);
    for (auto dot : dots) {
        float dx = board.inner_x + (board.inner_gap * dot[0]) + 0.5;
        float dy = board.inner_y + (board.inner_gap * dot[1]) + 0.5;
        DrawFilledCircle(renderer, dx, dy, 3);
    }
}

void GoDrawHelper::DrawText (TTF_TextEngine* text_engine, TTF_Font* font, std::pair<int, int> point, std::string str, int font_size, GoTextAlign align) {
    TTF_SetFontSize(font, font_size);
    TTF_Text *text = TTF_CreateText(text_engine, font, str.c_str(), str.size());

    int w, h;
    TTF_GetTextSize(text, &w, &h);

    TTF_SetTextColor(text, 200, 200, 200, 255);
    if (align == GoTextAlign::LEFT_ALIGN) {
        TTF_DrawRendererText(text, point.first, point.second);
    } else if (align == GoTextAlign::MIDDLE_ALIGN) {
        TTF_DrawRendererText(text, point.first - (w/2.f), point.second);
    } else if (align == GoTextAlign::RIGHT_ALIGN) {
        TTF_DrawRendererText(text, point.first - w, point.second);
    }

    TTF_DestroyText(text);
}

