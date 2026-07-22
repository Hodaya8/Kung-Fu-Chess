#pragma once

#include <optional>
#include <vector>
#include "model/piece_removed_info.hpp"
#include "engine/GameEngine.hpp"
#include "input/BoardMapper.hpp"
#include "input/Controller.hpp"
#include "model/board.hpp"
#include "model/game_snapshot.hpp"
#include "model/piece.hpp"
#include "model/position.hpp"

class GameSession
{
private:
    static constexpr int BOARD_ROWS = 8;
    static constexpr int BOARD_COLUMNS = 8;

    Board board;
    GameEngine engine;
    BoardMapper mapper;

    Controller whiteController;
    Controller blackController;

    static Board createInitialBoard();

    Controller& controllerFor(
        Color playerColor
    );

    const Controller& controllerFor(
        Color playerColor
    ) const;

public:
    GameSession();

    void handleLeftClick(
        Color playerColor,
        int x,
        int y
    );

    void handleRightClick(
        Color playerColor,
        int x,
        int y
    );

    std::vector<PieceRemovedInfo> advanceTime(int milliseconds);

    GameSnapshot getSnapshot() const;

    bool isGameOver() const;

    std::optional<Position>
    getSelectedPosition(
        Color playerColor
    ) const;
};