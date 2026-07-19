#pragma once

#include <vector>
#include <string>
#include "position.hpp"
#include "piece.hpp"
#include <memory>

class Board
{
private:
    std::vector<std::vector<std::shared_ptr<Piece>>> cells;
public:
    Board() = default;
    Board(const std::vector<std::vector<std::shared_ptr<Piece>>>& cells);

    int rows() const;
    int cols() const;
    const std::shared_ptr<Piece>& at(int row, int col) const;
    bool hasPiece(Position pos) const;

    void setPiece(int row, int col, std::shared_ptr<Piece> piece);

    void movePiece(Position source, Position destination);
};