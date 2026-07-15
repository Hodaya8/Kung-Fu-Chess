#include <gtest/gtest.h>
#include "model/board.hpp"
#include "rules/KingRule.hpp"
#include "rules/KnightRule.hpp"
#include "rules/RookRule.hpp"

TEST(PieceRulesTest, KingRuleLogic) {
    KingRule king;
    Board board; // לוח ריק לבדיקת תנועה יחסית
    EXPECT_TRUE(king.canMove(board, Position(4,4), Position(5,5)));
    EXPECT_FALSE(king.canMove(board, Position(4,4), Position(4,6)));
}

TEST(PieceRulesTest, KnightRuleLogic) {
    KnightRule knight;
    Board board;
    EXPECT_TRUE(knight.canMove(board, Position(4,4), Position(6,5)));
    EXPECT_FALSE(knight.canMove(board, Position(4,4), Position(5,5)));
}

TEST(PieceRulesTest, RookRuleLogic) {
    RookRule rook;
    std::vector<std::vector<std::string>> grid(8, std::vector<std::string>(8, "."));
    grid[4][5] = "bP"; // חסימה
    Board board(grid);
    
    EXPECT_TRUE(rook.canMove(board, Position(4,4), Position(7,4))); // פנוי
    EXPECT_FALSE(rook.canMove(board, Position(4,4), Position(4,7))); // חסום
}