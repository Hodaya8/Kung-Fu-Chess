#include <gtest/gtest.h>
#include "rules/PawnRule.hpp"
#include "model/Board.hpp"

TEST(PawnRuleTests, MovesForward) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    board.setPiece(2, 0, "wP"); // שורת התחלה של לבן בלוח 4x4
    PawnRule pawn;

    EXPECT_TRUE(pawn.canMove(board, Position(2,0), Position(1,0)));
}

TEST(PawnRuleTests, DoubleStepInvalidFromNonStart) {
    std::vector<std::vector<std::string>> cells(4, std::vector<std::string>(4, "."));
    Board board(cells);
    board.setPiece(3, 1, "wP"); // שורה שאינה התחלה
    PawnRule pawn;

    EXPECT_FALSE(pawn.canMove(board, Position(3,1), Position(1,1)));
}

