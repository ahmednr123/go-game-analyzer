#include "katago.hpp"
#include "actions.hpp"
#include "base.hpp"
#include "error.hpp"
#include "helpers.hpp"
#include "katago_engine.hpp"
#include "katago_settings.hpp"
#include "utils.hpp"
#include <SDL3/SDL_log.h>
#include <cassert>
#include <memory>
#include <optional>

std::vector<std::string> parseMove (json response) {
    return {
        response["rootInfo"]["currentPlayer"],
        response["moveInfos"][0]["move"]
    };
}

std::vector<std::vector<std::string>> getMoves (GoBoardSize size, std::vector<GoBoardAction> actions) {
    std::vector<std::vector<std::string>> moves;
    for (int i = 0; i < actions.size(); i++) {
        auto action = actions[i];

        // Can be a GoStone or GoTurn -> GoTurn is pass
        std::optional<std::variant<GoStone, GoTurn>> stone_opt =
            std::visit([&](auto&& action) -> std::optional<std::variant<GoStone, GoTurn>> {
                using T = std::decay_t<decltype(action)>;

                if constexpr (std::is_same_v<T, AddStoneAction>) {
                    return std::make_optional(action.stone);
                }
                else if constexpr (std::is_same_v<T, CaptureStonesAction>) {
                    return std::make_optional(action.capturing_stone);
                }
                else if constexpr (std::is_same_v<T, PassAction>) {
                    return std::make_optional(action.turn);
                }

                return std::nullopt;
            }, action);

        if (stone_opt.has_value()) {
            Result<std::vector<std::string>, GoErrorEnum> move_opt =
                std::visit([&](auto&& go_move) -> Result<std::vector<std::string>, GoErrorEnum> {
                    using T = std::decay_t<decltype(go_move)>;

                    if constexpr (std::is_same_v<T, GoStone>) {
                        return Ok(stoneToKatagoMove(size, go_move));
                    }
                    else if constexpr (std::is_same_v<T, GoTurn>) {
                        return Ok(std::vector<std::string>{
                            go_move == GoTurn::BLACK ?
                                "B" : "W",
                            "pass"
                        });
                    }

                    return Err(GoErrorEnum::UNPARSEABLE_MOVE);
                }, stone_opt.value());

            if (move_opt.is_ok()) {
                moves.push_back(move_opt.ok_value());
            }
        }
    }
    return moves;
}

KataGo::KataGo(
    const std::string& katago_path,
    const std::string& config_path,
    const std::string& model_path,
    GoBoardSize size
) {
    this->engine =
        std::unique_ptr<KataGoEngine>(
            new KataGoEngine(
                katago_path,
                config_path,
                model_path,
                [&](bool is_failure) {
                    is_init_failure = is_failure;
                }
            )
        );
    this->size = size;
}

void KataGo::updateDiffLevel (int diff_lvl) {
    assert((diff_lvl >= 1 && diff_lvl <= 5) && "Difficulty level out of range. [Accepted range: 1-5]");
    this->diff_lvl = diff_lvl;
}

std::optional<std::variant<GoStone, GoTurn>>
KataGo::nextNMoves (
    std::vector<GoBoardAction> actions, int n
) {
    if (is_init_failure)
        return std::nullopt;

    std::optional<std::variant<GoStone, GoTurn>> go_move_opt = std::nullopt;
    std::lock_guard<std::mutex> lock(work_mutex);
    try {
        is_busy.store(true);
        std::vector<std::vector<std::string>> moves =
            getMoves(this->size, actions);

        while (n--) {
            std::string id = genRandomString(5);
            json query = getMoveQuery(id, moves, size);
            addDiffLevelToKatagoRequest(query, this->diff_lvl);

            engine->sendJSON(query);

            std::optional<std::vector<std::string>>
                next_move_opt = engine->getNextMove();

            if (!next_move_opt.has_value()) {
                return std::nullopt;
            }

            std::vector<std::string> next_move = next_move_opt.value();
            moves.push_back(next_move);

            if (next_move[1] != "pass") {
                GoStone stone = katagoMoveToStone(this->size, next_move);
                std::cout << "Move: " << next_move[0] << next_move[1] << ", x=" << stone.x << " y=" << stone.y << std::endl;
                go_move_opt = std::make_optional(stone);
            } else {
                std::cout << "Move: " << next_move[0] << " pass" << std::endl;
                go_move_opt = std::make_optional(
                    next_move[0] == "B" ?
                        GoTurn::BLACK :
                        GoTurn::WHITE
                );
            }
        }

    } catch (const json::parse_error& err) {
        std::cerr << "Parse error for invalid JSON: " << err.what() << std::endl;
        std::cerr << "Error details: " << err.id << " at position " << err.byte << std::endl;
    }

    is_busy.store(false);
    return go_move_opt;
}

std::optional<KataGoEvaluation>
KataGo::getEvaluation (std::vector<GoBoardAction> actions) {
    if (is_init_failure)
        return std::nullopt;

    std::lock_guard<std::mutex> lock(work_mutex);
    std::optional<KataGoEvaluation> evaluation = std::nullopt;
    try {
        std::vector<std::vector<std::string>> moves =
            getMoves(this->size, actions);

        std::string id = genRandomString(5);
        json query = getEvaluationQuery(id, moves, size);

        engine->sendJSON(query);

        evaluation = engine->getEvaluation();
    } catch (const json::parse_error& err) {
        std::cerr << "Parse error for invalid JSON: " << err.what() << std::endl;
        std::cerr << "Error details: " << err.id << " at position " << err.byte << std::endl;
    }

    return evaluation;
}

 bool KataGo::isBusy () {
    return is_busy.load();
}

