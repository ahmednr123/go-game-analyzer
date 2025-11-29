#include "state.hpp"
#include "actions.hpp"
#include "base.hpp"
#include "compute.hpp"
#include "error.hpp"
#include "rules.hpp"
#include "sound.hpp"
#include <optional>
#include <iostream>
#include <variant>

void GoBoardState::handleUndoClear () {
    if (undo_by > 0 && actions.size() >= undo_by) {
        actions.resize(actions.size()-undo_by);
    }
    undo_by = 0;
}

void GoBoardState::clear () {
    actions.clear();
    undo_by = 0;

    this->computed = GoBoardStateComputed(static_cast<int>(dim));
}

Result<bool, GoErrorEnum> GoBoardState::redo () {
    if (undo_by-1 >= 0) {
        undo_by--;

        Result<GoBoardStateComputed, GoErrorEnum> res =
            computeActions(
                this->dim,
                std::vector(
                    this->actions.begin(),
                    this->actions.end() - undo_by
                )
            );
        if (res.is_err()) {
            return Err(res.err_value());
        }

        this->computed = res.ok_value();
        return Ok(true);
    }

    return Ok(false);
}

Result<bool, GoErrorEnum> GoBoardState::undo () {
    if (static_cast<long>(actions.size()) - (undo_by+1) >= 0) {
        undo_by++;

        Result<GoBoardStateComputed, GoErrorEnum> res =
            computeActions(
                this->dim,
                std::vector(
                    this->actions.begin(),
                    this->actions.end() - undo_by
                )
            );
        if (res.is_err()) {
            return Err(res.err_value());
        }

        this->computed = res.ok_value();
        return Ok(true);
    }

    return Ok(false);
}

Result<bool, GoErrorEnum> GoBoardState::pass (GoTurn turn) {
    if (this->computed.isGameEnded())
        return Ok(false);

    std::optional<GoTurn> prev_turn = std::nullopt;

    if (actions.size() > 0) {
        auto action = actions[actions.size()-undo_by-1];
        prev_turn =
            std::visit([&](auto&& action) -> std::optional<GoTurn> {
                using T = std::decay_t<decltype(action)>;

                if constexpr (std::is_same_v<T, PassAction>) {
                    GoTurn turn = action.turn;
                    return std::make_optional(turn);
                }

                return std::nullopt;
            }, action);
    }

    if (!prev_turn.has_value() || (prev_turn.has_value() && prev_turn.value() != turn)) {
        actions.push_back(PassAction({turn}));
        Result<GoBoardStateComputed, GoErrorEnum> res =
            computeActions(
                this->dim,
                std::vector(
                    this->actions.begin(),
                    this->actions.end() - undo_by
                )
            );
        if (res.is_err())
            return Err(res.err_value());

        this->computed = res.ok_value();
        return Ok(true);
    }


    return Ok(false);
}

Result<bool, GoErrorEnum> GoBoardState::addStone (GoStone stone) {
    if (this->computed.isGameEnded())
        return Ok(false);

    GoBoardCellState cell_state = this->computed.get(stone.x, stone.y);
    if (cell_state != GoBoardCellState::EMPTY) {
        return Ok(false);
    }

    bool is_stone_added = false;
    std::vector<std::vector<GoStone>> captured_groups =
        GoBoardRuleManager::getCapturedGroups(this->computed, stone);

    if (captured_groups.size() > 0) {
        GoSound::playCapture();

        std::vector<GoStone> removed_stones;
        for (auto group : captured_groups) {
            removed_stones.insert(removed_stones.end(), group.begin(), group.end());
        }
        this->handleUndoClear();
        actions.push_back(
            CaptureStonesAction({
                stone, removed_stones
            })
        );
        is_stone_added = true;
    } else if (GoBoardRuleManager::isValidStoneIgnoringCapture(this->computed, stone)) {
        this->handleUndoClear();
        actions.push_back(
            AddStoneAction({stone})
        );
        is_stone_added = true;
    }

    if (is_stone_added) {
        Result<GoBoardStateComputed, GoErrorEnum> res =
            computeActions(
                this->dim,
                std::vector(
                    this->actions.begin(),
                    this->actions.end() - undo_by
                )
            );
        if (res.is_err()) {
            return Err(res.err_value());
        }

        this->computed = res.ok_value();
    }

    return Ok(is_stone_added);
}

int GoBoardState::getCaptures (GoTurn turn) {
    int captures = 0;
    for (int i = 0; i < actions.size() - undo_by; i++) {
        auto action = actions[i];

        captures +=
            std::visit([&](auto&& action) -> int {
                using T = std::decay_t<decltype(action)>;

                if constexpr (std::is_same_v<T, CaptureStonesAction>) {
                    if (action.capturing_stone.turn == turn)
                        return action.removed_stones.size();
                }

                return 0;
            }, action);
    }
    return captures;
}
