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

    for (int i = 0; i < actions.size(); i++) {
        auto action = actions[i];

        std::optional<GoErrorEnum> err =
            std::visit([&](auto&& action) -> std::optional<GoErrorEnum> {
                using T = std::decay_t<decltype(action)>;

                if constexpr (std::is_same_v<T, AddStoneAction>) {
                    GoStone stone = action.stone;
                    state[stone.x][stone.y] =
                        stone.turn == GoTurn::BLACK ?
                            GoBoardCellState::BLACK :
                            GoBoardCellState::WHITE;
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

#endif
