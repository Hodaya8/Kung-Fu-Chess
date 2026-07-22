#include "Controller.hpp"
 
//constructor
Controller::Controller(Board& board, BoardMapper& mapper, GameEngine& engine, Color playerColor)
:
//רשימת אתחול (מיד ביצירה)
board(board),
mapper(mapper),  //המתרגם מפיקסלים למשבצות
engine(engine),    //אליו ישלחו הפקודות
playerColor(playerColor),
hasSelection(false),
selected(0,0)
{
}

bool Controller::isPlayerPiece(
    const Position& cell) const
{
    const auto piece =
        board.at(
            cell.getRow(),
            cell.getCol()
        );

    return
        piece &&
        piece->getColor() ==
            playerColor;
}

void Controller::selectCell(
    const Position& cell)
{
    selected = cell;
    hasSelection = true;
}



void Controller::click(int x, int y)
{
    // לחיצה מחוץ ללוח
    if(!mapper.insideBoard(x, y))
    {
        hasSelection = false;
        return;
    }

    Position cell = mapper.pixelToCell(x, y);   //המרה למשבצת

    // אין כרגע כלי נבחר
    if(!hasSelection)
    {
         if (isPlayerPiece(cell))
        {
            selectCell(cell);
        }

        return;
    }

    if (!isPlayerPiece(selected))
    {
        hasSelection=false;
        return;
    }
    
    // לחיצה כפולה על אותו כלי יוצרת בקשת Jump
    if (cell == selected)
    {
        engine.requestJump(cell);
        hasSelection=false;
        return;
    }
    // לחיצה על כלי אחר של השחקן מחליפה בחירה.
    if (isPlayerPiece(cell))
    {
        selectCell(cell);
        return;
    }
    engine.requestMove(
        selected,
        cell
    );
    hasSelection = false;
    
}

void Controller::jump(int x, int y)
{
    if(!mapper.insideBoard(x, y)) return;

    Position cell = mapper.pixelToCell(x, y);

    if (!isPlayerPiece(cell))
    {
        return;
    }

    engine.requestJump(cell);
}

bool Controller::isSelected() const
{
    return hasSelection;
}

Position Controller::getSelected() const
{
    return selected;
}