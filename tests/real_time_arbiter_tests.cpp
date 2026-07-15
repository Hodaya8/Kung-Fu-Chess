#include <gtest/gtest.h>
#include "realtime/RealTimeArbiter.hpp"
#include "model/board.hpp"

TEST(RealTimeArbiterTest, MotionAndAdvanceTime) {
    RealTimeArbiter arbiter;
    std::vector<std::vector<std::string>> grid(8, std::vector<std::string>(8, "."));
    grid[0][0] = "wR";
    grid[0][3] = "bP";
    Board board(grid);
    
    arbiter.addMotion(Motion(Position(0,0), Position(0,3), 2000));
    EXPECT_TRUE(arbiter.has_active_motion());
    
    arbiter.advance_time(1000, board); // אמצע הדרך
    EXPECT_EQ(board.at(0,0), "wR");
    
    std::string captured = arbiter.advance_time(1000, board); // הגעה ליעד
    EXPECT_EQ(captured, "bP");
    EXPECT_EQ(board.at(0,0), ".");
    EXPECT_EQ(board.at(0,3), "wR");
}
TEST(RealTimeArbiterTest, JumpSavesFromCaptureAndKillsAttacker) {
    std::vector<std::vector<std::string>> grid = {
        {"wR", ".", ".", "bR"}
    };
    Board board(grid);
    RealTimeArbiter arbiter;

    // צריח לבן יוצא לתקוף צריח שחור (יגיע בזמן 3000)
    arbiter.addMotion(Motion(Position(0,0), Position(0,3), 3000));
    
    // נקדם את הזמן ל-2800. השחור קופץ באוויר ל-500ms (יינחת ב-3300)
    arbiter.advance_time(2800, board);
    arbiter.addJump(Position(0,3), 500); 
    
    // קידום ל-3000 (הלבן מגיע כשהשחור באוויר)
    std::string captured = arbiter.advance_time(200, board);
    
    // השחור היה באוויר ולכן הוא זה שקוטל את הלבן התוקף!
    EXPECT_EQ(captured, "wR"); 
    EXPECT_EQ(board.at(0,0), "."); // הלבן נעלם מהלוח
}

TEST(RealTimeArbiterTest, JumpExpiresNormally) {
    std::vector<std::vector<std::string>> grid = { {"wK"} };
    Board board(grid);
    RealTimeArbiter arbiter;

    arbiter.addJump(Position(0,0), 1000);
    EXPECT_TRUE(arbiter.isPieceJumping(Position(0,0)));

    arbiter.advance_time(1001, board);
    EXPECT_FALSE(arbiter.isPieceJumping(Position(0,0))); // הזמן עבר, הכלי נחת
}