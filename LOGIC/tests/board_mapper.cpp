#include <gtest/gtest.h>
#include "input/BoardMapper.hpp"

TEST(BoardMapperTest, ConversionAndBoundaries) {
    BoardMapper mapper(8, 8);
    
    EXPECT_TRUE(mapper.insideBoard(50, 50));
    EXPECT_FALSE(mapper.insideBoard(850, 50)); // גודל תא = 100
    
    Position p = mapper.pixelToCell(250, 150); // x=250(col 2), y=150(row 1)
    EXPECT_EQ(p.getRow(), 1);
    EXPECT_EQ(p.getCol(), 2);
}