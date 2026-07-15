#include "rules/BishopRule.hpp"
#include <cstdlib>


bool BishopRule::canMove(
    const Board& board,
    Position from,
    Position to
) const
{

    int rowDiff =
        abs(from.getRow() - to.getRow());


    int colDiff =
        abs(from.getCol() - to.getCol());


    if(rowDiff != colDiff)
        return false;

    int rowStep =
        (to.getRow() > from.getRow()) ? 1 : -1;


    int colStep =
        (to.getCol() > from.getCol()) ? 1 : -1;



    int row =
        from.getRow() + rowStep;


    int col =
        from.getCol() + colStep;



    while(
        row != to.getRow()
    )
    {

        if(board.at(row,col) != ".")
            return false;


        row += rowStep;
        col += colStep;
    }


    return true;
}
