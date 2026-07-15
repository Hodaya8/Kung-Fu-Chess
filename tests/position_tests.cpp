#include <gtest/gtest.h>
#include "model/position.hpp"

TEST(PositionTest, GettersAndEquality) {
    Position p1(2, 3);
    Position p2(2, 3);
    Position p3(3, 2);
    
    EXPECT_EQ(p1.getRow(), 2);
    EXPECT_EQ(p1.getCol(), 3);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
}