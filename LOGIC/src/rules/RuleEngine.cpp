#include "rules/RuleEngine.hpp"

#include <functional>
#include <map>

#include "rules/RookRule.hpp"
#include "rules/BishopRule.hpp"
#include "rules/QueenRule.hpp"
#include "rules/KnightRule.hpp"
#include "rules/KingRule.hpp"
#include "rules/PawnRule.hpp"

// פונקציה המחזירה את חוק התנועה המתאים לכלי
std::unique_ptr<PieceRule>
RuleEngine::getRule(
    std::shared_ptr<Piece> piece) const
{
    static const std::map<
        PieceType,
        std::function<
            std::unique_ptr<PieceRule>()
        >
    > rulesMap = {
        {
            PieceType::ROOK,
            []()
            {
                return std::make_unique<RookRule>();
            }
        },
        {
            PieceType::BISHOP,
            []()
            {
                return std::make_unique<BishopRule>();
            }
        },
        {
            PieceType::QUEEN,
            []()
            {
                return std::make_unique<QueenRule>();
            }
        },
        {
            PieceType::KNIGHT,
            []()
            {
                return std::make_unique<KnightRule>();
            }
        },
        {
            PieceType::KING,
            []()
            {
                return std::make_unique<KingRule>();
            }
        },
        {
            PieceType::PAWN,
            []()
            {
                return std::make_unique<PawnRule>();
            }
        }
    };

    auto ruleIterator =
        rulesMap.find(
            piece->getType()
        );

    if (ruleIterator !=
        rulesMap.end())
    {
        return ruleIterator->second();
    }

    return nullptr;
}

// בדיקה האם התנועה חוקית
bool RuleEngine::isLegalMove(
    std::shared_ptr<Piece> piece,
    Position from,
    Position to,
    const Board& board) const
{
    auto rule = getRule(piece);

    if (!rule)
    {
        return false;
    }

    if (!rule->canMove(
            board,
            from,
            to))
    {
        return false;
    }

    auto destinationPiece =
        board.at(
            to.getRow(),
            to.getCol()
        );

    if (!destinationPiece)
    {
        return true;
    }

    return piece->getColor() !=
           destinationPiece->getColor();
}

// החזרת זמן התנועה של הכלי
int RuleEngine::getTravelTime(
    std::shared_ptr<Piece> piece,
    Position from,
    Position to) const
{
    auto rule = getRule(piece);

    if (!rule)
    {
        return 0;
    }

    return rule->calculateTravelTime(
        from,
        to
    );
}

// בדיקה האם המשחק הסתיים
bool RuleEngine::isGameOver(
    PieceType removedPieceType) const
{
    return removedPieceType ==
           PieceType::KING;
}