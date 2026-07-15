#include "rules/PawnRule.hpp"
#include <cstdlib>

bool PawnRule::canMove(const Board& board, Position from, Position to) const
{
    std::string piece = board.at(from.getRow(), from.getCol());
    char color = piece[0]; 
    int expectedRowDiff = (color == 'w') ? -1 : 1;
    
    // שורת ההתחלה היא תמיד השורה השנייה מהקצה של אותו צד
    int startRow = (color == 'w') ? (board.rows() - 2) : 1;

    int rowDiff = to.getRow() - from.getRow();
    int colDiff = to.getCol() - from.getCol();

    // לוגיקת תנועה כפולה (צעד של 2 משבצות)
    if (colDiff == 0 && rowDiff == 2 * expectedRowDiff)
    {
        // אם החייל לא נמצא פיזית בשורת ההתחלה שלו - המהלך הכפול אסור!
        if (from.getRow() != startRow) return false;

        int midRow = from.getRow() + expectedRowDiff;
        if (board.at(midRow, from.getCol()) != ".") return false;
        if (board.at(to.getRow(), to.getCol()) != ".") return false;

        return true;
    }

    if (rowDiff != expectedRowDiff) return false;

    std::string destPiece = board.at(to.getRow(), to.getCol());

    if (colDiff == 0) {
        return destPiece == ".";
    }

    if (std::abs(colDiff) == 1) {
        return destPiece != ".";
    }

    return false;
}