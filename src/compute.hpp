#ifndef GO_COMPUTE_H
#define GO_COMPUTE_H

#include "actions.hpp"
#include "base.hpp"
#include "state.hpp"
#include <optional>

inline bool doesCellValidateStone (GoBoardCellState cell_state, const GoStone& stone) {
    return (cell_state == GoBoardCellState::BLACK && stone.turn == GoTurn::BLACK)
        || (cell_state == GoBoardCellState::WHITE && stone.turn == GoTurn::WHITE);
}


inline Result<GoBoardStateComputed, GoErrorEnum>
computeActions (GoBoardSize size, std::vector<GoBoardAction> actions) {
    int board_dim = static_cast<int>(size);
    std::vector<std::vector<GoBoardCellState>> state(
            board_dim,
            std::vector<GoBoardCellState>(board_dim, GoBoardCellState::EMPTY)
    );

    bool is_game_ended = false;
    std::vector<GoTurn> passes = {};
    for (int i = 0; i < actions.size(); i++) {
        auto action = actions[i];

        if (is_game_ended) break;
        std::optional<GoErrorEnum> err =
            std::visit([&](auto&& action) -> std::optional<GoErrorEnum> {
                using T = std::decay_t<decltype(action)>;

                if constexpr (std::is_same_v<T, AddStoneAction>) {
                    GoStone stone = action.stone;
                    state[stone.x][stone.y] =
                        stone.turn == GoTurn::BLACK ?
                            GoBoardCellState::BLACK :
                            GoBoardCellState::WHITE;
                    passes.clear();
                }
                else if constexpr (std::is_same_v<T, CaptureStonesAction>) {
                    GoStone capturing_stone = action.capturing_stone;
                    state[capturing_stone.x][capturing_stone.y] =
                        capturing_stone.turn == GoTurn::BLACK ?
                            GoBoardCellState::BLACK :
                            GoBoardCellState::WHITE;

                    std::vector<GoStone> stones = action.removed_stones;
                    for (GoStone stone : stones) {
                        if (state[stone.x][stone.y] == GoBoardCellState::EMPTY
                                || !doesCellValidateStone(state[stone.x][stone.y], stone)
                        ){
                            return std::make_optional<GoErrorEnum>(GoErrorEnum::BOARD_STATE_CORRUPT);
                        }

                        state[stone.x][stone.y] = GoBoardCellState::EMPTY;
                    }
                    passes.clear();
                }
                else if constexpr (std::is_same_v<T, PassAction>) {
                    GoTurn turn = action.turn;
                    if (passes.size() > 0) {
                        if (passes[0] != turn) {
                            is_game_ended = true;
                        }
                    } else {
                        passes.push_back(turn);
                    }
                }

                return std::nullopt;
            }, action);

        if (err.has_value()) {
            return Err(err.value());
        }
    }

    return Ok(
        GoBoardStateComputed(
            state,
            passes.size() > 0 ?
                std::make_optional<GoTurn>(passes[0]) :
                std::nullopt,
            is_game_ended
        )
    );
}

inline bool computeIfKo (GoBoardSize size, std::vector<GoBoardAction> actions, CaptureStonesAction action) {
    // Need atleast 7 stones for a Ko
    if (actions.size() > 6) {
        actions.push_back(action);

        Result<GoBoardStateComputed, GoErrorEnum> curr =
            computeActions(
                size,
                actions
            );

        Result<GoBoardStateComputed, GoErrorEnum> prev =
            computeActions(
                size,
                std::vector(
                    actions.begin(),
                    actions.end() - 2
                )
            );

        if (curr.is_err() || prev.is_err()) {
            return false;
        }

        return curr.ok_value() == prev.ok_value();
    }
    return false;
}

#endif
