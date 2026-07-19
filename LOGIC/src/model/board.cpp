#include "model/board.hpp"
#include "model/piece.hpp"
#include <memory>
Board::Board(const std::vector<std::vector<std::shared_ptr<Piece>>>& cells) 
    : cells(cells) // השמה פשוטה
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

const std::shared_ptr<Piece>& Board::at(int row, int col) const
{
    return cells[row][col];
}

bool Board::hasPiece(Position pos) const
{
    return cells[pos.getRow()][pos.getCol()] != nullptr;
}

void Board::setPiece(int row, int col, std::shared_ptr<Piece> piece)
{
    if (row >= 0 && row < rows() && col >= 0 && col < cols()) {
        cells[row][col] = piece;
    }
}

void Board::movePiece(Position source, Position destination)
{
    
    cells[destination.getRow()][destination.getCol()] = cells[source.getRow()][source.getCol()];
    if (cells[destination.getRow()][destination.getCol()] != nullptr) {
        cells[destination.getRow()][destination.getCol()]->setPosition(destination);
    }
    cells[source.getRow()][source.getCol()] = nullptr;
}