#include "rules/PawnRule.hpp"
#include <cstdlib>

bool PawnRule::canMove(const Board& board, Position from, Position to) const
{
    auto piece = board.at(from.getRow(), from.getCol());
    if (!piece) return false;

    // 1. שימוש ב-getColor() במקום [0]
    Color color = piece->getColor();
    int expectedRowDiff = (color == Color::WHITE) ? -1 : 1;
    int startRow = (color == Color::WHITE) ? (board.rows() - 2) : 1;

    int rowDiff = to.getRow() - from.getRow();
    int colDiff = to.getCol() - from.getCol();

    // לוגיקת תנועה כפולה
    if (colDiff == 0 && rowDiff == 2 * expectedRowDiff)
    {
        if (from.getRow() != startRow) return false;
        int midRow = from.getRow() + expectedRowDiff;
        
        // עובד מצוין עם nullptr
        if (board.at(midRow, from.getCol()) != nullptr) return false;
        if (board.at(to.getRow(), to.getCol()) != nullptr) return false;
        return true;
    }

    if (rowDiff != expectedRowDiff) return false;

    // 2. עבודה ישירה עם האובייקט (לא string)
    auto destPiece = board.at(to.getRow(), to.getCol());

    if (colDiff == 0) {
        return destPiece == nullptr; // תקין
    }

    if (std::abs(colDiff) == 1) {
        // 3. תנועה אלכסונית דורשת אכילה: וודאי שהיעד לא ריק (אבל במציאות צריך גם לבדוק צבע)
        return destPiece != nullptr && destPiece->getColor() != color;
    }

    return false;
}