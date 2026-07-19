#pragma once

#include "model/Position.hpp"

class BoardMapper
{
private:
    static const int CELL_SIZE = 100;

    int boardRows;
    int boardCols;

public:

    BoardMapper(int rows, int cols);

    bool insideBoard(int x, int y) const;

    Position pixelToCell(int x, int y) const;
};