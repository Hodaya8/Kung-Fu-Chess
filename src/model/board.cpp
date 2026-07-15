#include "model/board.hpp"

Board::Board(const std::vector<std::vector<std::string>>& board)
    : cells(board)
{
}

int Board::rows() const
{
    return cells.size();
}

int Board::cols() const
{
    if (cells.empty()) return 0;
    return cells[0].size();
}

const std::string& Board::at(int row, int col) const
{
    return cells[row][col];
}

bool Board::hasPiece(Position pos) const
{
    return cells[pos.getRow()][pos.getCol()] != ".";
}

// מימוש הפונקציה החדשה
void Board::setPiece(int row, int col, const std::string& piece)
{
    if (row >= 0 && row < rows() && col >= 0 && col < cols()) {
        cells[row][col] = piece;
    }
}

void Board::movePiece(Position source, Position destination)
{
    cells[destination.getRow()][destination.getCol()] = cells[source.getRow()][source.getCol()];
    cells[source.getRow()][source.getCol()] = ".";
}