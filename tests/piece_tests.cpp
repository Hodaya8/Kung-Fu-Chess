#include <gtest/gtest.h>
#include "model/piece.hpp"

TEST(PieceTest, InitializationAndGetters) {
    Position pos(1, 1);
    Piece p(10, Color::WHITE, PieceType::KNIGHT, pos);
    
    EXPECT_EQ(p.getId(), 10);
    EXPECT_EQ(p.getColor(), Color::WHITE);
    EXPECT_EQ(p.getType(), PieceType::KNIGHT);
    EXPECT_EQ(p.getState(), PieceState::IDLE);
    
    p.setPosition(Position(2, 3));
    EXPECT_TRUE(p.getPosition() == Position(2, 3));
}