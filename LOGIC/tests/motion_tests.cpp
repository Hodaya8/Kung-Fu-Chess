#include <gtest/gtest.h>
#include "realtime/Motion.hpp"

TEST(MotionTest, Lifecycle) {
    Motion m(Position(0,0), Position(2,2), 5000);
    EXPECT_TRUE(m.getSource() == Position(0,0));
    EXPECT_TRUE(m.getDestination() == Position(2,2));
    EXPECT_EQ(m.getArrivalTime(), 5000);
    
    EXPECT_FALSE(m.isFinished(4000));
    EXPECT_TRUE(m.isFinished(5000));
}