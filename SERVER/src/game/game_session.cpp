#include "game/game_session.hpp"

#include <sstream>
#include <string>

#include "io/board_parser.hpp"

Board GameSession::createInitialBoard()
{
    const std::string initialBoard =
        "Board:\n"
        "bR bN bB bQ bK bB bN bR\n"
        "bP bP bP bP bP bP bP bP\n"
        ". . . . . . . .\n"
        ". . . . . . . .\n"
        ". . . . . . . .\n"
        ". . . . . . . .\n"
        "wP wP wP wP wP wP wP wP\n"
        "wR wN wB wQ wK wB wN wR\n";

    std::stringstream stream(
        initialBoard
    );

    return BoardParser::parse(stream);
}

GameSession::GameSession()
    : board(createInitialBoard()),
      engine(board),
      mapper(
          BOARD_ROWS,
          BOARD_COLUMNS
      ),
      controller(
          board,
          mapper,
          engine
      )
{
}

void GameSession::handleLeftClick(
    int x,
    int y)
{
    controller.click(x, y);
}

void GameSession::handleRightClick(
    int x,
    int y)
{
    controller.jump(x, y);
}

void GameSession::advanceTime(
    int milliseconds)
{
    engine.wait(milliseconds);
}

GameSnapshot GameSession::getSnapshot() const
{
    return engine.getSnapshot();
}

bool GameSession::isGameOver() const
{
    return engine.isGameOver();
}

std::optional<Position>
GameSession::getSelectedPosition() const
{
    if (!controller.isSelected())
    {
        return std::nullopt;
    }

    return controller.getSelected();
}