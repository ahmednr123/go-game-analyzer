#include "katago.hpp"
#include "actions.hpp"
#include "base.hpp"
#include "error.hpp"
#include "helpers.hpp"
#include "utils.hpp"
#include <SDL3/SDL_log.h>
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
): engine(katago_path, config_path, model_path) {
    this->size = size;
}

void KataGo::nextNMoves (
    std::vector<GoBoardAction> actions,
    int n, std::function<void(std::variant<GoStone, GoTurn>)> func
) {
    try {
        is_busy = true;
        std::vector<std::vector<std::string>> moves =
            getMoves(this->size, actions);

        while (n--) {
            std::string id = genRandomString(5);
            json query = getMoveQuery(id, moves, size);
            std::cout << query.dump() << std::endl;

            engine.sendJSON(query);

            std::vector<std::string> next_move = engine.getNextMove();
            moves.push_back(next_move);

            if (func != nullptr) {
                if (next_move[1] != "pass") {
                    GoStone stone = katagoMoveToStone(this->size, next_move);
                    std::cout << "Move: " << next_move[0] << next_move[1] << ", x=" << stone.x << " y=" << stone.y << std::endl;
                    func(stone);
                } else {
                    std::cout << "Move: " << next_move[0] << " pass" << std::endl;
                    func(
                        next_move[0] == "B" ?
                            GoTurn::BLACK :
                            GoTurn::WHITE
                    );
                }
            }
        }

    } catch (const json::parse_error& err) {
        std::cerr << "Parse error for invalid JSON: " << err.what() << std::endl;
        std::cerr << "Error details: " << err.id << " at position " << err.byte << std::endl;
    }

    is_busy = false;
}

std::optional<std::vector<std::vector<double>>>
KataGo::getEvaluation (std::vector<GoBoardAction> actions) {
    try {
        is_busy = true;
        std::vector<std::vector<std::string>> moves =
            getMoves(this->size, actions);

        std::string id = genRandomString(5);
        std::cout << "GENERATING EVALUATION QUERY" << std::endl;
        json query = getEvaluationQuery(id, moves, size);
        std::cout << query.dump() << std::endl;

        engine.sendJSON(query);

        return engine.getOwnership();
    } catch (const json::parse_error& err) {
        std::cerr << "Parse error for invalid JSON: " << err.what() << std::endl;
        std::cerr << "Error details: " << err.id << " at position " << err.byte << std::endl;
    }

    is_busy = false;
    return std::nullopt;
}

bool KataGo::isBusy () {
    return is_busy;
}

