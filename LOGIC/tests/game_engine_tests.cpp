#include <gtest/gtest.h>

#include "engine/GameEngine.hpp"
#include "model/board.hpp"
#include "model/game_snapshot.hpp"

// בדיקה שהכתרת רגלי מתבצעת אוטומטית
TEST(GameEngineTest, RequestMoveAndPromotion)
{
    std::vector<std::vector<std::string>> grid(
        8,
        std::vector<std::string>(8, ".")
    );

    grid[1][0] = "wP";

    Board board(grid);
    GameEngine engine(board);

    engine.requestMove(
        Position(1, 0),
        Position(0, 0)
    );

    engine.wait(2000);

    auto promotedPiece =
        board.at(0, 0);

    ASSERT_NE(
        promotedPiece,
        nullptr
    );

    EXPECT_EQ(
        promotedPiece->getType(),
        PieceType::QUEEN
    );

    EXPECT_EQ(
        promotedPiece->getColor(),
        Color::WHITE
    );

    EXPECT_EQ(
        board.at(1, 0),
        nullptr
    );
}

// בדיקה שאי אפשר ליצור שתי תנועות לאותו כלי
TEST(GameEngineTest, RejectsMoveWhenMotionIsActive)
{
    std::vector<std::vector<std::string>> grid = {
        {
            "wR",
            ".",
            "."
        }
    };

    Board board(grid);
    GameEngine engine(board);

    engine.requestMove(
        Position(0, 0),
        Position(0, 1)
    );

    // בקשה נוספת לאותו כלי בזמן שהוא כבר בתנועה
    engine.requestMove(
        Position(0, 0),
        Position(0, 2)
    );

    GameSnapshot snapshot =
        engine.getSnapshot();

    int movingPieces = 0;

    for (const auto& piece :
         snapshot.getPieces())
    {
        if (piece.state ==
            PieceState::move)
        {
            ++movingPieces;
        }
    }

    EXPECT_EQ(
        movingPieces,
        1
    );
}