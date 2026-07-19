#pragma once

#include "model/position.hpp"
#include "model/board.hpp"

class PieceRule
{

public:
// פונקציה הבודקת אם הכלי יכול לבצע תנועה מוסימת
//הפונקציה יכולה לקבל מימוש שונה במחלקות  והן חייבות לממש אותה
    virtual bool canMove(
        const Board& board,
        Position from,  //נקודת התחלה
        Position to     //יעד
    ) const = 0;
    //חישוב מהירות הכלי
    virtual int calculateTravelTime(
        Position from,
        Position to
    ) const;
    //destructure
    virtual ~PieceRule() = default;

};