#include "board.hpp"
#include "base.hpp"
#include "config.hpp"
#include "draw.hpp"
#include "error.hpp"
#include "katago.hpp"
#include "sound.hpp"
#include "state.hpp"
#include "theme.hpp"
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

    int board_dim = static_cast<int>(dim);
    this->board = GetGoBoardInfo(w, h, dim);
    this->katago_evaluation = {0, std::vector<std::vector<double>>(board_dim, std::vector<double>(board_dim, 0))};
}

void GoBoard::setupTextEngine(TTF_TextEngine* text_engine, TTF_Font* font) {
    this->text_engine = text_engine;
    this->font = font;
}

void GoBoard::updateBoardInfo(int w, int h) {
    board = GetGoBoardInfo(w, h, dim);
}

void GoBoard::handleGoMove(std::variant<GoStone, GoTurn> go_move) {
    if (katago.isBusy()) return;
    std::visit([&](auto &&go_move) {
        using T = std::decay_t<decltype(go_move)>;
        if constexpr (std::is_same_v<T, GoStone>) {
            Result<bool, GoErrorEnum> res = state->addStone(go_move);
            if (res.is_err()) {
                SDL_Log("Add stone error");
            }

            if (res.is_ok() && res.ok_value()) {
                GoSound::playTap();
                if (auto_switch_flag && turn == go_move.turn) {
                    turn = turn == GoTurn::WHITE ?
                        GoTurn::BLACK :
                        GoTurn::WHITE;
                }
                std::thread([&]() {
                    auto katago_evaluation_opt = katago.getEvaluation(state->getActionsWithUndo());
                    if (katago_evaluation_opt.has_value()) {
                        katago_evaluation = katago_evaluation_opt.value();
                    }
                }).detach();
            }
        }
        else if constexpr (std::is_same_v<T, GoTurn>) {
            Result<bool, GoErrorEnum> res = state->pass(go_move);
            if (res.is_err()) {
                SDL_Log("Pass error");
            }

            if (res.is_ok() && res.ok_value()) {
                GoSound::playTap();
                if (auto_switch_flag && turn == go_move) {
                    turn = turn == GoTurn::WHITE ?
                        GoTurn::BLACK :
                        GoTurn::WHITE;
                }
                std::thread([&]() {
                    auto katago_evaluation_opt = katago.getEvaluation(state->getActionsWithUndo());
                    if (katago_evaluation_opt.has_value()) {
                        katago_evaluation = katago_evaluation_opt.value();
                    }
                }).detach();
            }
        }
    }, go_move);
}

void GoBoard::handleEvent(SDL_Event* event, const std::vector<GoError>& errors) {
    std::optional<GoErrorSeverity> error_severity_opt =
        GoErrorHandler::getErrorSeverity(errors);
    if (error_severity_opt.has_value()
            && error_severity_opt.value() <= GoErrorSeverity::UNRECOVERABLE) {
        return;
    }

    switch (event->type) {
        case SDL_EVENT_KEY_DOWN: {
            SDL_KeyboardEvent key_event = event->key;
            if (key_event.scancode == SDL_SCANCODE_V) {
                this->view_ownership = true;
            }

            break;
        }
        case SDL_EVENT_KEY_UP: {
            SDL_KeyboardEvent key_event = event->key;

            if (key_event.mod & SDL_KMOD_SHIFT) {
                if (key_event.scancode == SDL_SCANCODE_R) {
                    this->state->clear();
                }
            }

            if (error_severity_opt.has_value()
                    && error_severity_opt.value() >= GoErrorSeverity::RECOVERABLE) {
                break;
            }

            if (key_event.scancode == SDL_SCANCODE_M) {
                GoSound::toggleMusic();
            } else if (key_event.scancode == SDL_SCANCODE_T) {
                GoThemeHandler::nextTheme();
            } else if (key_event.scancode == SDL_SCANCODE_S) {
                this->show_text = !this->show_text;
            } else if (key_event.scancode == SDL_SCANCODE_V) {
                this->view_ownership = false;
            } else if (key_event.scancode == SDL_SCANCODE_C) {
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
                    std::thread([&]() {
                        auto katago_evaluation_opt = katago.getEvaluation(state->getActionsWithUndo());
                        if (katago_evaluation_opt.has_value()) {
                            katago_evaluation = katago_evaluation_opt.value();
                        }
                    }).detach();
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
                    std::thread([&]() {
                        auto katago_evaluation_opt = katago.getEvaluation(state->getActionsWithUndo());
                        if (katago_evaluation_opt.has_value()) {
                            katago_evaluation = katago_evaluation_opt.value();
                        }
                    }).detach();
                }
            } else if (key_event.scancode == SDL_SCANCODE_P) {
                this->handleGoMove(this->turn);
            } else if (key_event.scancode == SDL_SCANCODE_SPACE) {
                if (!this->state->getComputed().isGameEnded()) {
                    auto go_move_opt = this->katago.nextNMoves(this->state->getActionsWithUndo(), 1);
                    if (go_move_opt.has_value()) {
                        this->handleGoMove(go_move_opt.value());
                    } else {
                        GoErrorHandler::throwError(GoErrorEnum::ENGINE_NOT_FOUND);
                    }
                }
            } else if (key_event.scancode == SDL_SCANCODE_5) {
                this->katago.updateDiffLevel(5);
            } else if (key_event.scancode == SDL_SCANCODE_4) {
                this->katago.updateDiffLevel(4);
            } else if (key_event.scancode == SDL_SCANCODE_3) {
                this->katago.updateDiffLevel(3);
            } else if (key_event.scancode == SDL_SCANCODE_2) {
                this->katago.updateDiffLevel(2);
            } else if (key_event.scancode == SDL_SCANCODE_1) {
                this->katago.updateDiffLevel(1);
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            if (error_severity_opt.has_value()
                    && error_severity_opt.value() >= GoErrorSeverity::RECOVERABLE) {
                break;
            }

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

                    GoStone stone = {this->turn, x, y};
                    this->handleGoMove(stone);
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

    if (this->view_ownership) {
        for (int x = 0; x < board_dim; x++) {
            for (int y = 0; y < board_dim; y++) {
                GoDrawHelper::DrawOwnershipCell(
                    this->renderer,
                    this->board, {x, y},
                    this->katago_evaluation.ownership[x][y]
                );
            }
        }
    }
}

std::string getCapturesString (int black_captures, int white_captures) {
    return "Captures: B(" + std::to_string(black_captures) + ") W(" + std::to_string(white_captures) + ")";
}

void GoBoard::renderUI () {
    GoTheme theme = GoThemeHandler::getTheme();

    std::pair<int, int> top_left = {board.x, board.y - 20};
    std::pair<int, int> top_center = {board.x + (board.size/2), board.y - 20};
    std::pair<int, int> top_right = {board.x + board.size, board.y - 20};

    std::pair<int, int> bottom_left = {board.x, board.y + board.size + 6};
    std::pair<int, int> bottom_center = {board.x + (board.size/2), board.y + board.size + 6};
    std::pair<int, int> bottom_right = {board.x + board.size, board.y + board.size + 6};

    if (theme.hidden_board) {
        int piece_radius = board.inner_gap*0.65;

        top_left = {board.inner_x, board.y};
        top_center.second = board.y;
        top_right = {board.inner_x + board.inner_size, board.y};

        bottom_left = {board.inner_x, board.y + board.size - 10};
        bottom_center.second = board.y + board.size - 10;
        bottom_right = {board.inner_x + board.inner_size, board.y + board.size - 10};
    }

    if (this->show_text) {
        std::string auto_switch = "Auto Switch: ";
        auto_switch += auto_switch_flag ? "True" : "False";
        GoDrawHelper::DrawText(text_engine, font, theme.text_color, top_left, auto_switch, 12);

        if (!katago.isInitialized()) {
            GoDrawHelper::DrawText(
                text_engine, font, theme.error_text_color, top_center,
                "[Engine not found]", 12, GoTextAlign::MIDDLE_ALIGN
            );
        } else if (katago.isInitialized() && katago.isBusy()) {
            GoDrawHelper::DrawText(
                text_engine, font, theme.error_text_color, top_center,
                "[Engine Busy]", 12, GoTextAlign::MIDDLE_ALIGN
            );
        } else if (katago.isInitialized()){
            std::string diff_levels = " 5  4  3  2  1 ";
            int diff_lvl = this->katago.getDiffLevel();
            diff_levels[(5-diff_lvl)*3] = '[';
            diff_levels[(5-diff_lvl)*3 + 2] = ']';

            GoDrawHelper::DrawText(
                text_engine, font, theme.text_color, top_center,
                diff_levels, 12, GoTextAlign::MIDDLE_ALIGN
            );
        }

        GoDrawHelper::DrawText(text_engine, font, theme.text_color, top_right,
                turn == GoTurn::WHITE ? "White to play" : "Black to play", 12, GoTextAlign::RIGHT_ALIGN);

        std::string captures = getCapturesString(state->getCaptures(GoTurn::BLACK), state->getCaptures(GoTurn::WHITE));
        GoDrawHelper::DrawText(text_engine, font, theme.text_color, bottom_left, captures, 12);

        if (!this->state->getComputed().isGameEnded()) {
            std::optional<GoTurn> in_pass = this->state->getComputed().inPass();
            if (in_pass.has_value()) {
                GoDrawHelper::DrawText(
                    text_engine, font, theme.text_color, bottom_center,
                    in_pass.value() == GoTurn::WHITE ? "White Passes" : "Black Passes",
                    12, GoTextAlign::MIDDLE_ALIGN
                );
            }
        }

        if (katago.isInitialized()) {
            std::string score = "B+0";
            if (katago_evaluation.score > 0) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << katago_evaluation.score;
                score = "B+" + oss.str();
            } else if (katago_evaluation.score < 0) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << (katago_evaluation.score * -1);
                score = "W+" + oss.str();
            }
            GoDrawHelper::DrawText(
                text_engine, font, theme.text_color, bottom_right,
                score, 12, GoTextAlign::RIGHT_ALIGN
            );
        } else {
            GoDrawHelper::DrawText(
                text_engine, font, theme.text_color, bottom_right,
                "[Needs KataGo]", 12, GoTextAlign::RIGHT_ALIGN
            );
        }
    }

    if (this->state->getComputed().isGameEnded()) {
        GoDrawHelper::DrawText(
            text_engine, font, theme.error_text_color, bottom_center,
            "[GAME ENDED]", 12, GoTextAlign::MIDDLE_ALIGN
        );
    }
}
