#ifndef GO_KATAGO_H
#define GO_KATAGO_H

#include "actions.hpp"
#include "base.hpp"
#include "json.hpp"
#include "katago_engine.hpp"
#include "katago_settings.hpp"
#include <SDL3/SDL_log.h>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using nlohmann::json;

inline json getMoveQuery (
    std::string id,
    std::vector<std::vector<std::string>> moves,
    GoBoardSize size
) {
    int board_size =
        static_cast<int>(size);

    json req;

    req["id"] = id;
    req["moves"] = moves.size() > 0 ?
        moves :
        std::vector<std::vector<std::string>>(0);

    req["rules"] = "japanese";
    req["komi"] = 6.5;

    req["boardXSize"] = board_size;
    req["boardYSize"] = board_size;

    return req;
}

// Cache this? KataGo takes time to respond
// Better to jst calculate it yourself
inline json getEvaluationQuery (
    std::string id,
    std::vector<std::vector<std::string>> moves,
    GoBoardSize size
) {
    int board_size =
        static_cast<int>(size);

    json req;

    req["id"] = id;
    req["moves"] = moves.size() > 0 ?
        moves :
        std::vector<std::vector<std::string>>(0);

    req["rules"] = "japanese";
    req["komi"] = 6.5;

    req["boardXSize"] = board_size;
    req["boardYSize"] = board_size;

    req["maxVisits"] = 800;
    req["rootPolicyTemperature"] = 0.3;
    req["rootFpuReductionMax"] = 0.5;

    req["includeOwnership"] = true;

    return req;
}

std::vector<std::string> parseMove (json katago_resp);
std::vector<std::vector<std::string>> getMoves (GoBoardSize size, std::vector<GoBoardAction> actions);

inline GoStone katagoMoveToStone (GoBoardSize size, std::vector<std::string> move) {
    GoTurn turn = move[0] == "B" ? GoTurn::BLACK : GoTurn::WHITE;

    int y = move[1][0] - 'A';
    if (move[1][0] > 'I') y--;

    std::string x_string = std::string(move[1].begin()+1, move[1].end());
    int x = static_cast<int>(size) - std::stoi(x_string);

    return {turn, x, y};
}

inline std::vector<std::string> stoneToKatagoMove (GoBoardSize size, GoStone stone) {
    std::string pos;
    if (stone.y >= 8) stone.y++;
    pos.push_back('A' + stone.y);
    pos += std::to_string(static_cast<int>(size) - stone.x);
    return {
        stone.turn == GoTurn::BLACK ? "B" : "W",
        pos
    };
}

class KataGo {
private:
    GoBoardSize size;
    KataGoEngine engine;

    std::mutex work_mutex;
    std::atomic<bool> is_busy{false};

    int diff_lvl = 5; // 5,4,3,2,1

public:
    KataGo(
        const std::string& katago_path,
        const std::string& config_path,
        const std::string& model_path,
        GoBoardSize size
    );

    int getDiffLevel () { return diff_lvl; }
    void updateDiffLevel (int diff_lvl);

    std::optional<std::variant<GoStone, GoTurn>> nextNMoves (
        std::vector<GoBoardAction> actions, int n
    );

    std::optional<KataGoEvaluation>
    getEvaluation (std::vector<GoBoardAction> actions);

    bool isBusy ();
};

#endif

