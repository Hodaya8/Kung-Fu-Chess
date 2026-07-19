#include "input/BoardMapper.hpp"


BoardMapper::BoardMapper(int rows, int cols)
    :
    boardRows(rows),
    boardCols(cols)
{
}


bool BoardMapper::insideBoard(int x, int y) const
{
    int col = x / CELL_SIZE;
    int row = y / CELL_SIZE;


    return row >= 0 &&
           row < boardRows &&
           col >= 0 &&
           col < boardCols;
}



Position BoardMapper::pixelToCell(int x, int y) const
{
    return Position(
        y / CELL_SIZE,
        x / CELL_SIZE
    );
}