#ifndef GO_UNDO_H
#define GO_UNDO_H

#include "actions.hpp"
#include "helpers.hpp"
#include "error.hpp"
#include "base.hpp"
#include <SDL3/SDL_log.h>
#include <optional>
#include <variant>
#include <vector>

enum class GoBoardCellState {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

class GoBoardStateComputed {
    std::vector<std::vector<GoBoardCellState>> state;

public:
    GoBoardStateComputed (std::vector<std::vector<GoBoardCellState>> state): state(state) {};
    GoBoardCellState get (int x, int y) {
        return state[x][y];
    }
};

inline bool doesCellValidateStone (GoBoardCellState cell_state, const GoStone& stone) {
    return (cell_state == GoBoardCellState::BLACK && stone.turn != GoTurn::BLACK)
        || (cell_state == GoBoardCellState::WHITE && stone.turn != GoTurn::WHITE);
}

class GoBoardState {
    int undo_by = 0;
    std::vector<GoBoardAction> actions;

    GoBoardSize dim;

public:
    GoBoardState(GoBoardSize dim): dim(dim), actions(0) {}
    GoBoardState(std::vector<GoBoardAction> actions): actions(actions) {}

    void clear () {
        actions.clear();
        undo_by = 0;
    }

    void addAction (GoBoardAction action) {
        if (undo_by > 0 && actions.size() >= undo_by) {
            actions.resize(actions.size()-undo_by);
        }

        actions.push_back(action);
        undo_by = 0;
    }

    void undo () {
        if (actions.size() - (undo_by+1) >= 0) {
            undo_by++;
        }
    }

    Result<GoBoardStateComputed, GoErrorEnum> compute () {
        int board_dim = static_cast<int>(dim);
        std::vector<std::vector<GoBoardCellState>> state(
                board_dim,
                std::vector<GoBoardCellState>(board_dim, GoBoardCellState::EMPTY)
        );

        for (int i = 0; i < actions.size() - undo_by; i++) {
            auto action = actions[i];

            std::optional<GoErrorEnum> err =
                std::visit([&](auto&& action) -> std::optional<GoErrorEnum> {
                    using T = std::decay_t<decltype(action)>;

                    if constexpr (std::is_same_v<T, AddStone>) {
                        GoStone stone = action.stone;
                        state[stone.x][stone.y] =
                            stone.turn == GoTurn::BLACK ?
                                GoBoardCellState::BLACK :
                                GoBoardCellState::WHITE;
                    }
                    else if constexpr (std::is_same_v<T, RemoveStones>) {
                        std::vector<GoStone> stones = action.stones;
                        for (GoStone stone : stones) {
                            if (state[stone.x][stone.y] != GoBoardCellState::EMPTY
                                    || !doesCellValidateStone(state[stone.x][stone.y], stone)
                            ){
                                return std::make_optional(GoErrorEnum::BOARD_STATE_CORRUPT);
                            }

                            state[stone.x][stone.y] = GoBoardCellState::EMPTY;
                        }
                    }

                    return std::nullopt;
                }, action);

            if (err.has_value()) {
                return Err(err.value());
            }
        }

        return Ok(GoBoardStateComputed(state));
    }
};

#endif
