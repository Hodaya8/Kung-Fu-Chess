#include "input/BoardMapper.hpp"

BoardMapper::BoardMapper(
    int rows,
    int columns)
    : boardRows(rows),
      boardColumns(columns)
{
}

bool BoardMapper::insideBoard(
    int x,
    int y) const
{
    if (x < 0 || y < 0)
    {
        return false;
    }

    const int column =
        x / CELL_SIZE;

    const int row =
        y / CELL_SIZE;

    return
        row < boardRows &&
        column < boardColumns;
}

Position BoardMapper::pixelToCell(
    int x,
    int y) const
{
    return Position(
        y / CELL_SIZE,
        x / CELL_SIZE
    );
}