#pragma once

#include <optional>

#include "engine/GameEngine.hpp"
#include "input/BoardMapper.hpp"
#include "input/Controller.hpp"
#include "model/board.hpp"
#include "model/game_snapshot.hpp"
#include "model/position.hpp"

class GameSession
{
private:
    static constexpr int BOARD_ROWS = 8;
    static constexpr int BOARD_COLUMNS = 8;

    Board board;
    GameEngine engine;
    BoardMapper mapper;
    Controller controller;

    static Board createInitialBoard();

public:
    GameSession();

    void handleLeftClick(
        int x,
        int y
    );

    void handleRightClick(
        int x,
        int y
    );

    void advanceTime(
        int milliseconds
    );

    GameSnapshot getSnapshot() const;

    bool isGameOver() const;

    std::optional<Position>
    getSelectedPosition() const;
};