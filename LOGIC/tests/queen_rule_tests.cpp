#include <gtest/gtest.h>
#include "rules/QueenRule.hpp"
#include "model/Board.hpp"

TEST(QueenRuleTests, MovesLikeBishopAndRook) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    QueenRule queen;

    EXPECT_TRUE(queen.canMove(board, Position(0,0), Position(3,3))); // אלכסון
    EXPECT_TRUE(queen.canMove(board, Position(0,0), Position(0,3))); // ישר
}

TEST(QueenRuleTests, RejectsKnightMove) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    QueenRule queen;

    EXPECT_FALSE(queen.canMove(board, Position(0,0), Position(1,2)));
}