#pragma once

class Position
{
private:
    int row;
    int col;

public:
    Position(int r, int c);

    int getRow() const;
    int getCol() const;

    bool operator==(const Position& other) const;
};