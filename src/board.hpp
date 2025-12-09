#ifndef GO_BOARD_H
#define GO_BOARD_H

#include "base.hpp"
#include "katago.hpp"
#include "katago_engine.hpp"
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

    bool show_text = true;
    bool auto_switch_flag = true;
    GoTurn turn = GoTurn::BLACK;

    std::unique_ptr<KataGo> katago;
    std::shared_ptr<GoBoardState> state;

    KataGoEvaluation katago_evaluation;
    bool view_ownership = false;

    GoBoardInfo board;

public:
    GoBoard (SDL_Renderer* renderer, int w, int h, GoBoardSize dim);

    void setupTextEngine(TTF_TextEngine* text_engine, TTF_Font* font);
    void updateBoardInfo (int w, int h);

    void handleGoMove (std::variant<GoStone, GoTurn> go_move);

    void render();
    void handleEvent(SDL_Event* event, const std::vector<GoError>& errors);
    void renderUI();
};

#endif
