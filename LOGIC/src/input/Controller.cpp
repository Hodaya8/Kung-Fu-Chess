#include "Controller.hpp"
#include <memory>
 
//constructor
Controller::Controller(Board& board, BoardMapper& mapper, GameEngine& engine)
:
//רשימת אתחול (מיד ביצירה)
board(board),
mapper(mapper),  //המתרגם מפיקסלים למשבצות
engine(engine),    //אליו ישלחו הפקודות
hasSelection(false),
selected(0,0)
{
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
        if(board.hasPiece(cell))
        {
            selected = cell;
            hasSelection = true;
        }
        return;
    }
    
    // לחיצה כפולה על אותו כלי יוצרת בקשת Jump
    if (cell == selected)
    {
        engine.requestJump(cell);
        hasSelection = false;
        return;
    }
    auto currentPiece = board.at(cell.getRow(), cell.getCol());
    auto selectedPiece = board.at(selected.getRow(), selected.getCol());
    // אם הכלי השני הוא כלי מאותו צבע
    if(currentPiece != nullptr && selectedPiece != nullptr && 
       currentPiece->getColor() == selectedPiece->getColor())
    {
        selected = cell;
        hasSelection = true;
    }
    else
    {
        // אם התא ריק או מכיל כלי אויב - מבקשים תנועה (שתבצע אכילה במקרה של אויב)
        engine.requestMove(
            selected,
            cell
        );
        hasSelection = false;
    }
}

void Controller::jump(int x, int y)
{
    if(!mapper.insideBoard(x, y)) return;

    Position cell = mapper.pixelToCell(x, y);
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