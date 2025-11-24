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

inline bool isCellTurn (GoBoardCellState state, GoStone stone) {
    return (state == GoBoardCellState::BLACK && stone.turn == GoTurn::BLACK)
        || (state == GoBoardCellState::WHITE && stone.turn == GoTurn::WHITE);
}

class GoBoardStateComputed {
    std::vector<std::vector<GoBoardCellState>> state;

public:
    GoBoardStateComputed (int size): state(size, std::vector<GoBoardCellState>(size, GoBoardCellState::EMPTY)) {};
    GoBoardStateComputed (std::vector<std::vector<GoBoardCellState>> state): state(state) {};

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
    void addAction (GoBoardAction action);
    Result<bool, GoErrorEnum> undo ();
    Result<bool, GoErrorEnum> redo ();

    Result<GoBoardStateComputed, GoErrorEnum> compute ();
    GoBoardStateComputed getComputed () {
        return this->computed;
    }
};

#endif
