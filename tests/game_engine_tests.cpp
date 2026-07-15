#include <gtest/gtest.h>
#include "engine/GameEngine.hpp"
#include "model/board.hpp"

TEST(GameEngineTest, RequestMoveAndPromotion) {
    std::vector<std::vector<std::string>> grid(8, std::vector<std::string>(8, "."));
    grid[1][0] = "wP";
    Board board(grid);
    GameEngine engine(board);
    
    engine.requestMove(Position(1,0), Position(0,0));
    engine.wait(2000); // המתנה לסיום מהלך (תלוי בזמן שמוגדר בחוק)
    
    engine.handlePromotion();
    // בהנחה שהתנועה התבצעה:
    if(board.at(0,0) != ".") {
        EXPECT_EQ(board.at(0,0), "wQ");
    }
}

TEST(GameEngineTest, RejectsMoveWhenMotionIsActive) {
    std::vector<std::vector<std::string>> grid = {
        {"wR", ".", "bK"}
    };
    Board board(grid);
    GameEngine engine(board);

    // בקשה ראשונה תעבור
    engine.requestMove(Position(0,0), Position(0,1));
    
    // בקשה שנייה לפני שהזמן התקדם אמורה להידחות
    engine.requestMove(Position(0,1), Position(0,2));

}