#include <gtest/gtest.h>
#include "model/board.hpp"

TEST(BoardTest, GridOperations) {
    std::vector<std::vector<std::string>> grid = {
        {"wR", "."},
        {".", "bK"}
    };
    Board board(grid);
    
    EXPECT_EQ(board.rows(), 2);
    EXPECT_EQ(board.cols(), 2);
    EXPECT_EQ(board.at(0, 0), "wR");
    
    EXPECT_TRUE(board.hasPiece(Position(1, 1)));
    EXPECT_FALSE(board.hasPiece(Position(0, 1)));
    
    board.setPiece(0, 1, "wP");
    EXPECT_EQ(board.at(0, 1), "wP");
    
    board.movePiece(Position(1, 1), Position(1, 0));
    EXPECT_EQ(board.at(1, 1), ".");
    EXPECT_EQ(board.at(1, 0), "bK");
}