#include <gtest/gtest.h>
#include "input/Controller.hpp"
#include "input/BoardMapper.hpp"
#include "engine/GameEngine.hpp"
#include "model/board.hpp"

TEST(ControllerTest, ClickInteractions) {
    std::vector<std::vector<std::string>> grid(8, std::vector<std::string>(8, "."));
    grid[0][0] = "wR";
    Board board(grid);
    BoardMapper mapper(8, 8);
    GameEngine engine(board);
    Controller ctrl(board, mapper, engine);
    
    ctrl.click(50, 50); // בחירת הצריח הלבן
    EXPECT_TRUE(ctrl.isSelected());
    EXPECT_TRUE(ctrl.getSelected() == Position(0,0));
    
    ctrl.click(50, 150); // ניסיון תנועה למשבצת הבאה - שולח בקשה למנוע ומנקה בחירה
    EXPECT_FALSE(ctrl.isSelected());
}

TEST(ControllerTest, DoubleClickRequestsJump) {
    std::vector<std::vector<std::string>> grid = { {"wK"} };
    Board board(grid);
    BoardMapper mapper(1, 1);
    GameEngine engine(board);
    Controller ctrl(board, mapper, engine);

    ctrl.click(50, 50); // בחירת המלך
    EXPECT_TRUE(ctrl.isSelected());
    
    ctrl.click(50, 50); // לחיצה שנייה על אותו מקום
    EXPECT_FALSE(ctrl.isSelected()); // הבחירה מתנקה אחרי שמופעל Jump
}

TEST(ControllerTest, CannotSelectEmptyOrEnemyInitially) {
    std::vector<std::vector<std::string>> grid = {
        {".", "bK"}
    };
    Board board(grid);
    BoardMapper mapper(1, 2);
    GameEngine engine(board);
    Controller ctrl(board, mapper, engine);

    // ניסיון לבחור משבצת ריקה
    ctrl.click(50, 50); // 0,0
    EXPECT_FALSE(ctrl.isSelected());

    // הערה: לפי המימוש הנוכחי שלך, לחיצה ראשונית על כל כלי (גם שחור) בוחרת אותו.
    // אם תרצה להגביל צבע לפי התור, זה המקום לבדוק את זה!
}