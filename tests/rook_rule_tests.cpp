#include <gtest/gtest.h>
#include "rules/RookRule.hpp"
#include "model/Board.hpp"

TEST(RookRuleTests, MovesStraight) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    RookRule rook;

    EXPECT_TRUE(rook.canMove(board, Position(0,0), Position(0,3))); // אופקי
    EXPECT_TRUE(rook.canMove(board, Position(0,0), Position(3,0))); // אנכי
}

TEST(RookRuleTests, RejectsDiagonal) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    RookRule rook;

    EXPECT_FALSE(rook.canMove(board, Position(0,0), Position(1,1)));
}