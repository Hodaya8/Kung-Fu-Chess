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

    return BoardParser::parse(
        stream
    );
}

GameSession::GameSession()
    : board(createInitialBoard()),
      engine(board),
      mapper(
          BOARD_ROWS,
          BOARD_COLUMNS
      ),
      whiteController(
          board,
          mapper,
          engine,
          Color::WHITE
      ),
      blackController(
          board,
          mapper,
          engine,
          Color::BLACK
      )
{
}

Controller& GameSession::controllerFor(
    Color playerColor)
{
    return playerColor == Color::WHITE
        ? whiteController
        : blackController;
}

const Controller& GameSession::controllerFor(
    Color playerColor) const
{
    return playerColor == Color::WHITE
        ? whiteController
        : blackController;
}

void GameSession::handleLeftClick(
    Color playerColor,
    int x,
    int y)
{
    controllerFor(
        playerColor
    ).click(
        x,
        y
    );
}

void GameSession::handleRightClick(
    Color playerColor,
    int x,
    int y)
{
    controllerFor(
        playerColor
    ).jump(
        x,
        y
    );
}

std::vector<PieceRemovedInfo>
GameSession::advanceTime(
    int milliseconds)
{
    return engine.wait(
        milliseconds
    );
}

GameSnapshot GameSession::getSnapshot() const
{
    return engine.getSnapshot();
}

bool GameSession::isGameOver() const
{
    return engine.isGameOver();
}

std::optional<Color>
GameSession::getWinnerColor() const
{
    return engine.getWinnerColor();
}

std::optional<Position>
GameSession::getSelectedPosition(
    Color playerColor) const
{
    const Controller& controller =
        controllerFor(
            playerColor
        );

    if (!controller.isSelected())
    {
        return std::nullopt;
    }

    return controller.getSelected();
}