#include "game/game_room.hpp"

#include <sstream>
#include <string>
#include <utility>

#include "io/board_parser.hpp"

Board GameRoom::createInitialBoard()
{
    // String representation of a standard 8x8 chessboard setup
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

    std::stringstream stream(initialBoard);
    return BoardParser::parse(stream);
}

GameRoom::GameRoom(int roomId)
    : roomId(roomId),
      board(createInitialBoard()),
      engine(board),
      gameEndEventPublished(false)
{
    // Subscribe ScoreService to listen for whenever a piece is captured
    pieceRemovedBus.subscribe(
        [this](const PieceRemovedInfo& removedPiece)
        {
            scoreService.handlePieceRemoved(removedPiece);
        }
    );
}

int GameRoom::getId() const
{
    return roomId;
}

bool GameRoom::insideBoard(const Position& position) const
{
    return position.getRow() >= 0 && position.getRow() < board.rows() &&
           position.getCol() >= 0 && position.getCol() < board.cols();
}

//checks if the tool belong to the player trying to move it
bool GameRoom::isPlayerPiece(const Position& position, Color playerColor) const
{
    if (!insideBoard(position))
    {
        return false;
    }

    const auto piece = board.at(position.getRow(), position.getCol());

    // Check if the piece exists and matches the color of the player requesting the move
    return piece && piece->getColor() == playerColor;
}

void GameRoom::requestMove(Color playerColor, Position source, Position destination)
{
    if (engine.isGameOver())
    {
        return;
    }

    if (!insideBoard(source) || !insideBoard(destination))
    {
        return;
    }

    // Security check: Players can only move their own pieces!
    if (!isPlayerPiece(source, playerColor))
    {
        return;
    }
    //logic
    engine.requestMove(source, destination);
}

void GameRoom::requestJump(Color playerColor, Position cell)
{
    if (engine.isGameOver())
    {
        return;
    }

    if (!isPlayerPiece(cell, playerColor))
    {
        return;
    }
    //logic
    engine.requestJump(cell);
}

std::vector<PieceRemovedInfo> GameRoom::advanceTime(int milliseconds)
{
    // Tell the game engine to tick the clock forward and return any captured pieces
    const auto removedPieces = engine.wait(milliseconds);

    // publishing captured pieces
    for (const auto& removedPiece : removedPieces)
    {
        pieceRemovedBus.publish(removedPiece);
    }

    // Check if any capture ended the game
    publishGameEndedIfNeeded();

    return removedPieces;
}

GameSnapshot GameRoom::getSnapshot() const
{
    return engine.getSnapshot();
}

bool GameRoom::isGameOver() const
{
    return engine.isGameOver();
}

int GameRoom::getWhiteScore() const
{
    return scoreService.getWhiteScore();
}

int GameRoom::getBlackScore() const
{
    return scoreService.getBlackScore();
}

//Allows classes outside the room to register for the end of game event
void GameRoom::subscribeToGameEnded(GameEndedHandler handler)
{
    gameEndedBus.subscribe(std::move(handler));
}

void GameRoom::publishGameEndedIfNeeded()
{
    if (!engine.isGameOver() || gameEndEventPublished)
    {
        return;
    }

    const auto winnerColor = engine.getWinnerColor();

    if (!winnerColor.has_value())
    {
        return;
    }

    gameEndEventPublished = true;
    gameEndedBus.publish(winnerColor.value());
}