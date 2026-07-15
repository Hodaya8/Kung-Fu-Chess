#include <gtest/gtest.h>
#include "rules/KingRule.hpp"
#include "model/Board.hpp"

TEST(KingRuleTests, MovesOneSquare) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    KingRule king;

    EXPECT_TRUE(king.canMove(board, Position(0,0), Position(1,1)));
    EXPECT_TRUE(king.canMove(board, Position(1,1), Position(1,2)));
}

TEST(KingRuleTests, RejectsTwoSquares) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    KingRule king;

    EXPECT_FALSE(king.canMove(board, Position(0,0), Position(2,2)));
}