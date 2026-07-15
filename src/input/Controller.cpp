#include "input/Controller.hpp"
#include <memory>

Controller::Controller(
    Board& board,
    BoardMapper& mapper,
    GameEngine& engine
)
:
board(board),
mapper(mapper),
engine(engine),
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

    Position cell = mapper.pixelToCell(x, y);

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
    // תיקון: מחליפים בחירה רק אם לחצנו על כלי אחר באותו הצבע!
    if(currentPiece != nullptr && selectedPiece != nullptr && 
       currentPiece->getColor() == selectedPiece->getColor())
    {
        selected = cell;
        hasSelection = true;
    }
    else
    {
        // אם התא ריק או מכיל כלי אויב - מבקשים תנועה (שתבצע אכילה)
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