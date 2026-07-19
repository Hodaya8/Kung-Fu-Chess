#include "rules/KingRule.hpp"
#include <cstdlib>

//משבצת אחת בכל כיוון
bool KingRule::canMove(
    const Board&, // נוסף כדי להתאים למחלקת האב, המלך אינו משתמש בלוח ישירות
    Position from,
    Position to
) const
{

    int rowDiff =
        abs(from.getRow() - to.getRow());


    int colDiff =
        abs(from.getCol() - to.getCol());


    return 
        rowDiff <= 1
        &&
        colDiff <= 1
        &&
        !(rowDiff == 0 && colDiff == 0);

}