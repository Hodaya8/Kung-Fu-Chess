#include "rules/RookRule.hpp"
#include <algorithm> // בשביל std::max
#include <cstdlib>   // בשביל std::abs


bool RookRule::canMove(
    const Board& board,
    Position from,
    Position to
) const
{

    bool sameRow =
        from.getRow() == to.getRow();


    bool sameCol =
        from.getCol() == to.getCol();



    if(!sameRow && !sameCol)
        return false;



    int rowStep = 0;
    int colStep = 0;



    if(to.getRow() > from.getRow())
        rowStep = 1;

    if(to.getRow() < from.getRow())
        rowStep = -1;


    if(to.getCol() > from.getCol())
        colStep = 1;

    if(to.getCol() < from.getCol())
        colStep = -1;



    int row =
        from.getRow() + rowStep;


    int col =
        from.getCol() + colStep;



    while(
        row != to.getRow() ||
        col != to.getCol()
    )
    {

        if(board.at(row,col) != nullptr)
            return false;


        row += rowStep;
        col += colStep;
    }


    return true;
}

