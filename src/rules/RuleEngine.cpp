#include "rules/RuleEngine.hpp"

#include "rules/RookRule.hpp"
#include "rules/BishopRule.hpp"
#include "rules/QueenRule.hpp"
#include "rules/KnightRule.hpp"
#include "rules/KingRule.hpp"
#include "rules/PawnRule.hpp"

// קבלת אובייקט של הכלי
std::unique_ptr<PieceRule> RuleEngine::getRule(
    const std::string& piece
) const
{
    char type = piece[1];


    switch(type)
    {
        //יצירת אובייקט המתאים לכלי שזה המקרה שלו
        case 'R':
            return std::make_unique<RookRule>();

        case 'B':
            return std::make_unique<BishopRule>();

        case 'Q':
            return std::make_unique<QueenRule>();

        case 'N':
            return std::make_unique<KnightRule>();

        case 'K':
            return std::make_unique<KingRule>();

        case 'P':
            return std::make_unique<PawnRule>();
    }


    return nullptr;
}


// בדיקת התנועה
bool RuleEngine::isLegalMove(
    const std::string& piece,
    Position from,
    Position to,
    const Board& board
) const
{

    auto rule = getRule(piece);


    if(rule == nullptr)
    {
        return false;
    }


    if(!rule->canMove(board,from,to))
    {
        return false;
    }

    std::string destination =
        board.at(
            to.getRow(),
            to.getCol()
        );


    // תא ריק
    if(destination == ".")
        return true;

    // אסור לאכול אותו צבע

    return piece[0] != destination[0];
    
}


int RuleEngine::getTravelTime(
    const std::string& piece,
    Position from,
    Position to
) const
{
    auto rule = getRule(piece);
    if (rule != nullptr)
    {
        return rule->calculateTravelTime(from, to);
    }
    return 0;
}

