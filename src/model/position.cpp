#include "model/position.hpp"


Position::Position(int row, int col)
    : row(row), col(col)
{

}


int Position::getRow() const
{
    return row;
}


int Position::getCol() const
{
    return col;
}


bool Position::operator==(const Position& other) const
{
    return row == other.row &&
           col == other.col;
}