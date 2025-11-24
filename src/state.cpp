#include "state.hpp"
#include "actions.hpp"
#include "error.hpp"
#include "rules.hpp"
#include <optional>
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

        Result<GoBoardStateComputed, GoErrorEnum> res = this->compute();
        if (res.is_err()) {
            return Err(res.err_value());
        }

        return Ok(true);
    }

    return Ok(false);
}

Result<bool, GoErrorEnum> GoBoardState::undo () {
    if (static_cast<long>(actions.size()) - (undo_by+1) >= 0) {
        undo_by++;

        Result<GoBoardStateComputed, GoErrorEnum> res = this->compute();
        if (res.is_err()) {
            return Err(res.err_value());
        }

        return Ok(true);
    }

    return Ok(false);
}

Result<bool, GoErrorEnum> GoBoardState::addStone (GoStone stone) {
    GoBoardCellState cell_state = this->computed.get(stone.x, stone.y);
    if (cell_state != GoBoardCellState::EMPTY) {
        return Ok(false);
    }

    bool is_stone_added = false;
    std::vector<std::vector<GoStone>> captured_groups =
        GoBoardRuleManager::getCapturedGroups(this->computed, stone);

    if (captured_groups.size() > 0) {
        std::vector<GoStone> removed_stones;
        for (auto group : captured_groups) {
            removed_stones.insert(removed_stones.end(), group.begin(), group.end());
        }
        this->handleUndoClear();
        actions.push_back(
            CaptureStones({
                stone, removed_stones
            })
        );
        is_stone_added = true;
    } else if (GoBoardRuleManager::isValidStoneIgnoringCapture(this->computed, stone)) {
        this->handleUndoClear();
        actions.push_back(
            AddStone({stone})
        );
        is_stone_added = true;
    }

    if (is_stone_added) {
        Result<GoBoardStateComputed, GoErrorEnum> res = this->compute();
        if (res.is_err()) {
            return Err(res.err_value());
        }
    }

    return Ok(is_stone_added);
}

void GoBoardState::addAction (GoBoardAction action) {
    bool is_valid =
        std::visit([&](auto&& action) -> bool {
            using T = std::decay_t<decltype(action)>;

            if constexpr (std::is_same_v<T, AddStone>) {
                // Check if stone already exists
                GoStone stone = action.stone;
                GoBoardCellState cell_state = computed.get(stone.x, stone.y);
                if (cell_state != GoBoardCellState::EMPTY) {
                    return false;
                }
            }
            else if constexpr (std::is_same_v<T, CaptureStones>) {
                // Same Check if stone already exists
                GoStone stone = action.capturing_stone;
                GoBoardCellState cell_state = computed.get(stone.x, stone.y);
                if (cell_state != GoBoardCellState::EMPTY) {
                    return false;
                }
            }

            return true;
        }, action);

    if (!is_valid) {
        return;
    }

    if (undo_by > 0 && actions.size() >= undo_by) {
        actions.resize(actions.size()-undo_by);
    }

    actions.push_back(action);
    undo_by = 0;

    // Compute now?
}
