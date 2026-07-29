#pragma once

#include <functional>
#include <vector>

#include "engine/GameEngine.hpp"
#include "events/event_bus.hpp"
#include "model/board.hpp"
#include "model/game_snapshot.hpp"
#include "model/piece.hpp"
#include "model/piece_removed_info.hpp"
#include "model/position.hpp"
#include "scoring/score_service.hpp"

class GameRoom
{
public:
    // Callback type definition for game-over notifications
    using GameEndedHandler = std::function<void(const Color&)>;

    // Constructor accepting a unique room ID
    explicit GameRoom(int roomId);

    // Returns the unique identifier of the room
    int getId() const;

    // Handles a move request from a player
    void requestMove(Color playerColor, Position source, Position destination);

    // Handles a jump request (a special KungFu Chess mechanic)
    void requestJump(Color playerColor, Position cell);

    // Advances the game clock, processes cooldowns, and broadcasts removed pieces
    std::vector<PieceRemovedInfo> advanceTime(int milliseconds);

    // Returns the current state of the board and pieces
    GameSnapshot getSnapshot() const;

    // Checks if the game has ended
    bool isGameOver() const;

    // Getters for current player scores
    int getWhiteScore() const;
    int getBlackScore() const;

    // Allows outside services to listen for the game ending
    void subscribeToGameEnded(GameEndedHandler handler);

private:
    int roomId = 0;

    Board board;
    GameEngine engine;
    ScoreService scoreService;

    // Event buses specific to this game room
    EventBus<PieceRemovedInfo> pieceRemovedBus;
    EventBus<Color> gameEndedBus;

    // Flag to ensure the game-ended event is published only once
    bool gameEndEventPublished = false;

    // Helper to generate the standard starting chess board
    static Board createInitialBoard();

    // Validates if a position is within the board limits (0-7 rows/cols)
    bool insideBoard(const Position& position) const;

    // Validates if a piece at a given position belongs to the requesting player
    bool isPlayerPiece(const Position& position, Color playerColor) const;

    // Checks if game is over and fires the end event if needed
    void publishGameEndedIfNeeded();
};