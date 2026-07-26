#pragma once

#include <functional>
#include <optional>

#include "input/BoardMapper.hpp"
#include "model/game_snapshot.hpp"
#include "model/piece.hpp"
#include "model/position.hpp"

class Controller
{
public:
    using MoveHandler =
        std::function<
            void(
                Position,
                Position
            )
        >;

    using JumpHandler =
        std::function<
            void(Position)
        >;

    Controller(
        BoardMapper& mapper,
        MoveHandler moveHandler,
        JumpHandler jumpHandler
    );

    void handleLeftClick(
        int x,
        int y,
        const GameSnapshot& snapshot,
        Color playerColor
    );

    void handleRightClick(
        int x,
        int y,
        const GameSnapshot& snapshot,
        Color playerColor
    );

    std::optional<Position>
    getSelectedPosition() const;

    void clearSelection();

private:
    BoardMapper& mapper;

    MoveHandler moveHandler;
    JumpHandler jumpHandler;

    std::optional<Position>
        selectedPosition;

    bool isPlayerPiece(
        const GameSnapshot& snapshot,
        const Position& cell,
        Color playerColor
    ) const;

    void selectCell(
        const Position& cell
    );
};