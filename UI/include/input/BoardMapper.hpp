#pragma once

#include "model/position.hpp"

class BoardMapper
{
private:
    static constexpr int CELL_SIZE = 100;

    int boardRows;
    int boardColumns;

public:
    BoardMapper(
        int rows,
        int columns
    );

    bool insideBoard(
        int x,
        int y
    ) const;

    Position pixelToCell(
        int x,
        int y
    ) const;
};