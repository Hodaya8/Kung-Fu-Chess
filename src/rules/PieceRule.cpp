#include "rules/PieceRule.hpp"
#include <algorithm> // בשביל std::max
#include <cstdlib>   // בשביל std::abs

int PieceRule::calculateTravelTime(Position from, Position to) const
{
    int rowDiff = std::abs(from.getRow() - to.getRow());
    int colDiff = std::abs(from.getCol() - to.getCol());
    int steps = std::max(rowDiff, colDiff);

    return steps * 1000;
}