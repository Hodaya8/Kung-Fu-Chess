#include <gtest/gtest.h>
#include "rules/BishopRule.hpp"
#include "model/Board.hpp"

TEST(BishopRuleTests, MovesDiagonal) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    BishopRule bishop;

    EXPECT_TRUE(bishop.canMove(board, Position(0,0), Position(3,3)));
}

TEST(BishopRuleTests, CannotMoveStraight) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    BishopRule bishop;

    EXPECT_FALSE(bishop.canMove(board, Position(0,0), Position(0,3)));
}