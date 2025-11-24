#ifndef GO_BOARD_H
#define GO_BOARD_H

#include "base.hpp"
#include "state.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_textengine.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

class GoBoard {
private:
    SDL_Renderer* renderer;
    TTF_TextEngine* text_engine;
    TTF_Font* font;

    int window_w, window_h;
    GoBoardSize dim;

    bool auto_switch_flag = true;
    GoTurn turn = GoTurn::BLACK;
    std::unique_ptr<GoBoardState> state;

    GoBoardInfo board;

public:
    GoBoard (SDL_Renderer* renderer, int w, int h, GoBoardSize dim);

    void setupTextEngine(TTF_TextEngine* text_engine, TTF_Font* font);
    void updateBoardInfo (int w, int h);

    void render();
    void handleEvent(SDL_Event* event);
    void renderUI();
};

#endif
