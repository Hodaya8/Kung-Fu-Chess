#include "rules/KnightRule.hpp"
#include <cstdlib>


bool KnightRule::canMove(
    const Board&,
    Position from,
    Position to
) const
{

    int rowDiff =
        abs(from.getRow() - to.getRow());


    int colDiff =
        abs(from.getCol() - to.getCol());


    return
        (rowDiff == 2 && colDiff == 1)
        ||
        (rowDiff == 1 && colDiff == 2);

}

