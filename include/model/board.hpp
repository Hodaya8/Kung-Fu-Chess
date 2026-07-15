#pragma once

#include <vector>
#include <string>
#include "position.hpp"

class Board
{
private:
    std::vector<std::vector<std::string>> cells;

public:
    Board() = default;
    Board(const std::vector<std::vector<std::string>>& board);

    int rows() const;
    int cols() const;
    const std::string& at(int row, int col) const;
    bool hasPiece(Position pos) const;

    void setPiece(int row, int col, const std::string& piece);

    void movePiece(Position source, Position destination);
};