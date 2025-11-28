#include "base.hpp"
#include "katago.hpp"
#include "rules.hpp"
#include "state.hpp"
#include <algorithm>
#include <iostream>
#include <optional>

inline bool areGroupsEqual(
    const std::vector<std::vector<GoStone>>& expected_groups,
    const std::vector<std::vector<GoStone>>& found_groups
) {
    // Copy because we will sort
    auto expected = expected_groups;
    auto found = found_groups;

    // Sort stones within each group using GoStone::operator<
    for (auto& g : expected) std::sort(g.begin(), g.end());
    for (auto& g : found)    std::sort(g.begin(), g.end());

    // Sort the groups lexicographically using vector<GoStone>'s builtin comparison
    std::sort(expected.begin(), expected.end());
    std::sort(found.begin(), found.end());

    return expected == found;
}

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"

inline void printTestResult (std::string test, bool test_res) {
    std::cout << test << ": ";
    if (test_res) {
        std::cout << COLOR_GREEN << "[PASSED]" << COLOR_RESET << "\n";
    } else {
        std::cout << COLOR_RED << "[FAILED]" << COLOR_RESET << "\n";
    }
}

#define _ GoBoardCellState::EMPTY
#define X GoBoardCellState::BLACK
#define O GoBoardCellState::WHITE

#define W GoTurn::WHITE
#define B GoTurn::BLACK

inline bool testCaptureGroups () {
    std::vector<std::vector<GoBoardCellState>> state =
    {
        {_, _, _, _, _, _, _, X, O},
        {_, _, X, _, _, _, _, _, _},
        {_, X, O, X, _, _, _, _, _},
        {_, X, O, X, X, O, _, _, _},
        {_, X, O, O, O, _, O, _, _},
        {_, X, O, X, X, _, _, _, _},
        {_, _, X, _, _, _, _, _, _},
        {_, _, _, _, _, X, X, _, _},
        {_, _, _, _, X, O, O, O, X},
    };

    std::vector<std::vector<GoStone>> expected_groups_0 = {
        {{W, 2, 2}, {W, 3, 2}, {W, 4, 2}, {W, 5, 2}, {W, 4, 3}, {W, 4, 4}}
    };

    std::vector<std::vector<GoStone>> groups_0 =
        GoBoardRuleManager::getCapturedGroups({state, std::nullopt, false}, {B, 4, 5});

    bool is_valid_test_0 = areGroupsEqual(expected_groups_0, groups_0);
    printTestResult("testCaptureGroups[0]", is_valid_test_0);

    // ========

    std::vector<std::vector<GoStone>> expected_groups_1 = {{{W, 0, 8}}};

    std::vector<std::vector<GoStone>> groups_1 =
        GoBoardRuleManager::getCapturedGroups({state, std::nullopt, false}, {B, 1, 8});

    bool is_valid_test_1 = areGroupsEqual(expected_groups_1, groups_1);
    printTestResult("testCaptureGroups[1]", is_valid_test_1);

    // ========

    std::vector<std::vector<GoStone>> expected_groups_2 = {
        {{W, 8, 5}, {W, 8, 6}, {W, 8, 7}}
    };

    std::vector<std::vector<GoStone>> groups_2 =
        GoBoardRuleManager::getCapturedGroups({state, std::nullopt, false}, {B, 7, 7});

    bool is_valid_test_2 = areGroupsEqual(expected_groups_2, groups_2);
    printTestResult("testCaptureGroups[2]", is_valid_test_2);

    std::cout << std::endl;

    return is_valid_test_0
        && is_valid_test_1
        && is_valid_test_2;
}

inline bool testValidPlacement () {
    std::vector<std::vector<GoBoardCellState>> state =
    {
        {X, _, _, _, _, _, _, X, _},
        {_, X, _, _, _, _, X, _, X},
        {X, _, _, O, _, _, X, O, X},
        {_, _, O, _, O, _, X, O, X},
        {_, _, _, O, _, _, X, O, X},
        {_, _, _, _, _, _, _, X, _},
        {_, O, _, _, _, _, _, _, _},
        {O, X, O, _, _, _, _, _, _},
        {X, _, X, O, _, _, _, _, _},
    };

    bool is_valid_test_0 =
        !GoBoardRuleManager::isValidStoneIgnoringCapture({state, std::nullopt, false}, {W, 1, 0});
    printTestResult("testValidPlacement[0]", is_valid_test_0);

    bool is_valid_test_1 =
        GoBoardRuleManager::isValidStone({state, std::nullopt, false}, {W, 8, 1});
    printTestResult("testValidPlacement[1]", is_valid_test_1);

    bool is_valid_test_2 =
        GoBoardRuleManager::isValidStoneIgnoringCapture({state, std::nullopt, false}, {W, 3, 3});
    printTestResult("testValidPlacement[2]", is_valid_test_2);

    bool is_valid_test_3 =
        !GoBoardRuleManager::isValidStoneIgnoringCapture({state, std::nullopt, false}, {W, 1, 7});
    printTestResult("testValidPlacement[3]", is_valid_test_3);

    std::cout << std::endl;

    return is_valid_test_0
        && is_valid_test_1
        && is_valid_test_2
        && is_valid_test_3;
}

inline bool testValidKatago () {
    GoStone stone = {GoTurn::BLACK, 4, 8};
    std::vector<std::string> move = stoneToKatagoMove(GoBoardSize::_9x9, stone);
    std::cout << move[0] << " " << move[1] << std::endl;

    bool is_valid_test_0 = move[1][0] == 'J';
    printTestResult("testValidKatago[0]", is_valid_test_0);

    GoStone cal_stone = katagoMoveToStone(GoBoardSize::_9x9, move);
    std::cout << (cal_stone.turn == GoTurn::BLACK ? "B" : "W") << " " << cal_stone.x << " " << cal_stone.y << std::endl;

    bool is_valid_test_1 = stone == cal_stone;
    printTestResult("testValidKatago[1]", is_valid_test_1);

    std::cout << std::endl;
    return is_valid_test_0
        && is_valid_test_1;
}

#undef W
#undef B

#undef _
#undef X
#undef O

inline bool isTestPassed () {
    bool is_test_passing = testCaptureGroups()
        && testValidPlacement()
        && testValidKatago();

    printTestResult("Test", is_test_passing);
    return is_test_passing;
};
