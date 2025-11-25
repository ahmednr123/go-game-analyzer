#ifndef GO_UNDO_H
#define GO_UNDO_H

#include "actions.hpp"
#include "helpers.hpp"
#include "error.hpp"
#include "base.hpp"
#include <SDL3/SDL_log.h>
#include <optional>
#include <vector>

enum class GoBoardCellState {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

inline std::optional<GoTurn> getTurnFromCellState (GoBoardCellState state) {
    if (state == GoBoardCellState::BLACK)
        return std::make_optional(GoTurn::BLACK);
    else if (state == GoBoardCellState::WHITE)
        return std::make_optional(GoTurn::WHITE);

    return std::nullopt;
}

class GoBoardStateComputed {
    std::vector<std::vector<GoBoardCellState>> state;

public:
    GoBoardStateComputed (int size): state(size, std::vector<GoBoardCellState>(size, GoBoardCellState::EMPTY)) {};
    GoBoardStateComputed (std::vector<std::vector<GoBoardCellState>> state): state(state) {};

    bool operator==(const GoBoardStateComputed& other) const {
        if (state.size() != other.state.size())
            return false;

        for (int x = 0; x < state.size(); x++) {
            for (int y = 0; y < state[x].size(); y++) {
                if (state[x][y] != other.state[x][y]) {
                    return false;
                }
            }
        }

        return true;
    }

    GoBoardCellState get (int x, int y) const {
        return state[x][y];
    }

    int getSize () const {
        return state.size();
    }
};

class GoBoardState {
    int undo_by = 0;
    std::vector<GoBoardAction> actions;

    GoBoardStateComputed computed;
    GoBoardSize dim;

    void handleUndoClear ();

public:
    GoBoardState(GoBoardSize dim): dim(dim), actions(0), computed(static_cast<int>(dim)) {}

    void clear ();
    Result<bool, GoErrorEnum> addStone (GoStone stone);
    Result<bool, GoErrorEnum> undo ();
    Result<bool, GoErrorEnum> redo ();

    GoBoardStateComputed getComputed () {
        return this->computed;
    }

    int getCaptures (GoTurn turn);
};

#endif
