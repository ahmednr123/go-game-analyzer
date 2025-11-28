#include "board.hpp"
#include "base.hpp"
#include "config.hpp"
#include "draw.hpp"
#include "error.hpp"
#include "katago.hpp"
#include "state.hpp"
#include "utils.hpp"
#include "rules.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3_ttf/SDL_textengine.h>
#include <memory>
#include <optional>
#include <string>

GoBoard::GoBoard (SDL_Renderer* renderer, int w, int h, GoBoardSize dim):
    katago(KataGo(
        GoGameConfig::getKatagoPath(),
        GoGameConfig::getKatagoConfigPath(),
        GoGameConfig::getModelPath(),
        dim
    ))
{
    this->dim = dim;
    this->window_w = w;
    this->window_h = h;
    this->renderer = renderer;

    this->state = std::make_shared<GoBoardState>(dim);
    this->turn = GoTurn::BLACK;
    this->auto_switch_flag = true;

    this->board = GetGoBoardInfo(w, h, dim);
}

void GoBoard::setupTextEngine(TTF_TextEngine* text_engine, TTF_Font* font) {
    this->text_engine = text_engine;
    this->font = font;
}

void GoBoard::updateBoardInfo(int w, int h) {
    board = GetGoBoardInfo(w, h, dim);
}

void GoBoard::handleEvent(SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_KEY_UP: {
            SDL_KeyboardEvent key_event = event->key;

            if (key_event.mod & SDL_KMOD_SHIFT) {
                if (key_event.scancode == SDL_SCANCODE_R) {
                    this->state->clear();
                }
            }

            if (key_event.scancode == SDL_SCANCODE_C) {
                this->turn = this->turn == GoTurn::WHITE ?
                    GoTurn::BLACK :
                    GoTurn::WHITE;
            } else if (key_event.scancode == SDL_SCANCODE_X) {
                this->auto_switch_flag = !this->auto_switch_flag;
            } else if (key_event.scancode == SDL_SCANCODE_U) {
                Result<bool, GoErrorEnum> res = this->state->undo();
                if (res.is_err()) {
                    SDL_Log("Undo error");
                }

                if (this->auto_switch_flag && res.is_ok() && res.ok_value()) {
                    this->turn = this->turn == GoTurn::WHITE ?
                        GoTurn::BLACK :
                        GoTurn::WHITE;
                }
            } else if (key_event.scancode == SDL_SCANCODE_R) {
                Result<bool, GoErrorEnum> res = this->state->redo();
                if (res.is_err()) {
                    SDL_Log("Redo error");
                }

                if (this->auto_switch_flag && res.is_ok() && res.ok_value()) {
                    this->turn = this->turn == GoTurn::WHITE ?
                        GoTurn::BLACK :
                        GoTurn::WHITE;
                }
            } else if (key_event.scancode == SDL_SCANCODE_P) {
                Result<bool, GoErrorEnum> res = this->state->pass(this->turn);
                if (res.is_err()) {
                    SDL_Log("Pass error");
                }

                if (res.is_ok() && res.ok_value()) {
                    if (this->auto_switch_flag) {
                        this->turn = this->turn == GoTurn::WHITE ?
                            GoTurn::BLACK :
                            GoTurn::WHITE;
                    }
                    //auto ownership_opt = this->katago.getEvaluation(this->state->getActions());
                    //if (ownership_opt.has_value()) {
                    //    this->ownership = ownership_opt.value();
                    //}
                }
            } else if (key_event.scancode == SDL_SCANCODE_SPACE) {
                auto _this = this;
                this->katago.nextNMoves(this->state->getActions(), 1, [&](std::variant<GoStone, GoTurn> go_move) {
                    std::visit([&](auto &&go_move) {
                        using T = std::decay_t<decltype(go_move)>;
                        if constexpr (std::is_same_v<T, GoStone>) {
                            Result<bool, GoErrorEnum> res = _this->state->addStone(go_move);
                            if (res.is_err()) {
                                SDL_Log("AI add stone error");
                            }

                            if (res.is_ok() && res.ok_value()) {
                                if (_this->auto_switch_flag && _this->turn == go_move.turn) {
                                    _this->turn = _this->turn == GoTurn::WHITE ?
                                        GoTurn::BLACK :
                                        GoTurn::WHITE;
                                }
                                //auto ownership_opt = this->katago.getEvaluation(this->state->getActions());
                                //if (ownership_opt.has_value()) {
                                //    this->ownership = ownership_opt.value();
                                //}
                            }
                        }
                        else if constexpr (std::is_same_v<T, GoTurn>) {
                            Result<bool, GoErrorEnum> res = _this->state->pass(go_move);
                            if (res.is_err()) {
                                SDL_Log("AI pass error");
                            }

                            if (res.is_ok() && res.ok_value()) {
                                if (_this->auto_switch_flag && _this->turn == go_move) {
                                    _this->turn = _this->turn == GoTurn::WHITE ?
                                        GoTurn::BLACK :
                                        GoTurn::WHITE;
                                }
                                //auto ownership_opt = this->katago.getEvaluation(this->state->getActions());
                                //if (ownership_opt.has_value()) {
                                //    this->ownership = ownership_opt.value();
                                //}
                            }
                        }
                    }, go_move);
                });
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            SDL_MouseButtonEvent mouse_event = event->button;
            if (mouse_event.button == SDL_BUTTON_LEFT) {
                std::optional<std::pair<int, int>> point_opt =
                    getBoardCellFromPoint(
                        this->board,
                        mouse_event.x,
                        mouse_event.y
                    );
                if (point_opt.has_value()) {
                    int x = point_opt->first;
                    int y = point_opt->second;

                    Result<bool, GoErrorEnum> res =
                        this->state->addStone({this->turn, x, y});

                    if (res.is_err()) {
                        // Stop rendering and display error
                        SDL_Log("Add stone error");
                    }

                    if (res.is_ok() && res.ok_value()) {
                        if (this->auto_switch_flag) {
                            this->turn = this->turn == GoTurn::WHITE ?
                                GoTurn::BLACK :
                                GoTurn::WHITE;
                        }
                        //auto ownership_opt = this->katago.getEvaluation(this->state->getActions());
                        //if (ownership_opt.has_value()) {
                        //    this->ownership = ownership_opt.value();
                        //}
                    }
                }
            }
            break;
        }
    }
}

void GoBoard::render () {
    GoDrawHelper::DrawBoard(renderer, board);
    float mouse_x, mouse_y;
    Uint32 button_state = SDL_GetMouseState(&mouse_x, &mouse_y);

    GoBoardStateComputed computed_state = this->state->getComputed();
    if (!computed_state.isGameEnded()) {
        std::optional<std::pair<int, int>> point_opt =
            getBoardCellFromPoint(this->board, mouse_x, mouse_y);
        if (point_opt.has_value()) {
            int x = point_opt->first;
            int y = point_opt->second;

            GoStone stone = {turn, x, y};
            GoBoardCellState cell_state = computed_state.get(x, y);
            if (cell_state == GoBoardCellState::EMPTY
                    && GoBoardRuleManager::isValidStone(computed_state, stone))
            {
                GoDrawHelper::DrawStone(renderer, board, stone, 128);
            }
        }
    }

    int board_dim = static_cast<int>(this->board.dim);
    for (int x = 0; x < board_dim; x++) {
        for (int y = 0; y < board_dim; y++) {
            GoBoardCellState cell_state = computed_state.get(x, y);
            if (cell_state != GoBoardCellState::EMPTY) {
                GoDrawHelper::DrawStone(
                    this->renderer,
                    this->board,
                    {
                        cell_state == GoBoardCellState::BLACK ?
                            GoTurn::BLACK :
                            GoTurn::WHITE,
                        x, y
                    }
                );
            }
        }
    }
}

std::string getCapturesString (int black_captures, int white_captures) {
    return "Captures: B(" + std::to_string(black_captures) + ") W(" + std::to_string(white_captures) + ")";
}

void GoBoard::renderUI () {
        std::string auto_switch = "Auto Switch: ";
        auto_switch += auto_switch_flag ? "True" : "False";
        GoDrawHelper::DrawText(text_engine, font, OFF_WHITE_COLOR, {board.x, board.y - 20}, auto_switch, 12);

        GoDrawHelper::DrawText(
            text_engine, font, OFF_WHITE_COLOR,
            {board.x + (board.size/2), board.y - 20},
            "5  4 [3] 2  1",
            12, GoTextAlign::MIDDLE_ALIGN
        );

        GoDrawHelper::DrawText(text_engine, font, OFF_WHITE_COLOR, {board.x + board.size, board.y - 20},
                turn == GoTurn::WHITE ? "White to play" : "Black to play", 12, GoTextAlign::RIGHT_ALIGN);

        std::string captures = getCapturesString(state->getCaptures(GoTurn::BLACK), state->getCaptures(GoTurn::WHITE));
        GoDrawHelper::DrawText(text_engine, font, OFF_WHITE_COLOR, {board.x, board.y + board.size + 6}, captures, 12);

        if (this->state->getComputed().isGameEnded()) {
            GoDrawHelper::DrawText(
                text_engine, font, RED_COLOR,
                {board.x + (board.size/2), board.y + board.size + 6},
                "[GAME ENDED]",
                12, GoTextAlign::MIDDLE_ALIGN
            );
        } else {
            std::optional<GoTurn> in_pass = this->state->getComputed().inPass();
            if (in_pass.has_value()) {
                GoDrawHelper::DrawText(
                    text_engine, font, OFF_WHITE_COLOR,
                    {board.x + (board.size/2), board.y + board.size + 6},
                    in_pass.value() == GoTurn::WHITE ? "White Passes" : "Black Passes",
                    12, GoTextAlign::MIDDLE_ALIGN
                );
            }
        }

        GoDrawHelper::DrawText(text_engine, font, OFF_WHITE_COLOR, {board.x + board.size, board.y + board.size + 6}, "W+20", 12, GoTextAlign::RIGHT_ALIGN);
}
