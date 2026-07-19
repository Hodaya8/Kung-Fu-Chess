#include <gtest/gtest.h>
#include "rules/KnightRule.hpp"
#include "model/Board.hpp"

TEST(KnightRuleTests, MovesInLShape) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    KnightRule knight;

    EXPECT_TRUE(knight.canMove(board, Position(0,0), Position(1,2)));
    EXPECT_TRUE(knight.canMove(board, Position(0,0), Position(2,1)));
}

TEST(KnightRuleTests, RejectsStraightMove) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    KnightRule knight;

    EXPECT_FALSE(knight.canMove(board, Position(0,0), Position(2,0)));
}