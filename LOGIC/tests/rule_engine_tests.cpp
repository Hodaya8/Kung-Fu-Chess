#include <gtest/gtest.h>
#include "rules/RuleEngine.hpp"
#include "model/board.hpp"

TEST(RuleEngineTest, LegalMoveValidation) {
    RuleEngine engine;
    std::vector<std::vector<std::string>> grid(8, std::vector<std::string>(8, "."));
    grid[4][4] = "wR";
    grid[4][7] = "wP"; // חבר לקבוצה באותו צבע
    Board board(grid);
    
    // מותר לנוע למשבצת ריקה
    EXPECT_TRUE(engine.isLegalMove("wR", Position(4,4), Position(4,5), board));
    // אסור לאכול כלי באותו צבע
    EXPECT_FALSE(engine.isLegalMove("wR", Position(4,4), Position(4,7), board));
    
    EXPECT_EQ(engine.getTravelTime("wR", Position(0,0), Position(0,3)), 3000);
}

TEST(RuleEngineTest, PawnCaptureLogic) {
    RuleEngine engine;
    
    // בדיקה 1: אכילה חוקית של חייל שחור כלפי מטה (משורה 1 לשורה 2)
    std::vector<std::vector<std::string>> grid = {
        {".", ".", "."},
        {".", "bP", "."}, // החייל השחור שלנו בשורה 1
        {"wP", ".", "wP"} // מטרות לבנות מחכות לו באלכסונים בשורה 2
    };
    Board board(grid);
    
    // תנועה של חייל שחור באלכסון לצורך אכילה היא חוקית
    EXPECT_TRUE(engine.isLegalMove("bP", Position(1,1), Position(2,0), board)); 
    EXPECT_TRUE(engine.isLegalMove("bP", Position(1,1), Position(2,2), board)); 
    
    // בדיקה 2: ניסיון אכילה באלכסון כשהמשבצת ריקה (אמור להיכשל)
    std::vector<std::vector<std::string>> gridEmpty = {
        {".", ".", "."},
        {".", "bP", "."},
        {".", ".", "."} 
    };
    Board boardEmpty(gridEmpty);
    
    EXPECT_FALSE(engine.isLegalMove("bP", Position(1,1), Position(2,0), boardEmpty));
}

TEST(RuleEngineTest, UnknownPieceReturnsFalse) {
    RuleEngine engine;
    Board board;
    // כלי לא מוכר אמור להחזיר false ולא לקרוס
    EXPECT_FALSE(engine.isLegalMove("wX", Position(0,0), Position(1,1), board));
}