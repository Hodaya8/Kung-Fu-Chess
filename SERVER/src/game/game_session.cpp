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
      engine(board)
{
}

bool GameSession::insideBoard(
    const Position& position) const
{
    return
        position.getRow() >= 0 &&
        position.getRow() < board.rows() &&
        position.getCol() >= 0 &&
        position.getCol() < board.cols();
}

bool GameSession::isPlayerPiece(
    const Position& position,
    Color playerColor) const
{
    if (!insideBoard(
            position))
    {
        return false;
    }

    const auto piece =
        board.at(
            position.getRow(),
            position.getCol()
        );

    return
        piece &&
        piece->getColor() ==
            playerColor;
}

void GameSession::requestMove(
    Color playerColor,
    Position source,
    Position destination)
{
    if (engine.isGameOver())
    {
        return;
    }

    if (!insideBoard(source) ||
        !insideBoard(destination))
    {
        return;
    }

    // השרת לא סומך על הלקוח:
    // רק כלי ששייך למשתמש יכול להישלח למנוע.
    if (!isPlayerPiece(
            source,
            playerColor))
    {
        return;
    }

    engine.requestMove(
        source,
        destination
    );
}

void GameSession::requestJump(
    Color playerColor,
    Position cell)
{
    if (engine.isGameOver())
    {
        return;
    }

    if (!isPlayerPiece(
            cell,
            playerColor))
    {
        return;
    }

    engine.requestJump(
        cell
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