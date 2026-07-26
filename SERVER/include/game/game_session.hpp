#pragma once

#include <optional>
#include <vector>

#include "engine/GameEngine.hpp"
#include "model/board.hpp"
#include "model/game_snapshot.hpp"
#include "model/piece.hpp"
#include "model/piece_removed_info.hpp"
#include "model/position.hpp"

class GameSession
{
private:
    Board board;
    GameEngine engine;

    static Board createInitialBoard();

    bool insideBoard(
        const Position& position
    ) const;

    bool isPlayerPiece(
        const Position& position,
        Color playerColor
    ) const;

public:
    GameSession();

    void requestMove(
        Color playerColor,
        Position source,
        Position destination
    );

    void requestJump(
        Color playerColor,
        Position cell
    );

    std::vector<PieceRemovedInfo>
    advanceTime(
        int milliseconds
    );

    GameSnapshot getSnapshot() const;

    bool isGameOver() const;

    std::optional<Color>
    getWinnerColor() const;
};