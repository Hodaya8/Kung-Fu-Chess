#include "rules/RuleEngine.hpp"

#include "rules/RookRule.hpp"
#include "rules/BishopRule.hpp"
#include "rules/QueenRule.hpp"
#include "rules/KnightRule.hpp"
#include "rules/KingRule.hpp"
#include "rules/PawnRule.hpp"
#include <map>
#include <functional>

//פונקצייה המחזירה אובייקט של הכלי, משמשת את isLegalMove
std::unique_ptr<PieceRule> RuleEngine::getRule(std::shared_ptr<Piece> piece) const
{
    // המפה מכילה מפתח של אות הכלי וערך של פונקציית יצירתו
    static const std::map<PieceType, std::function<std::unique_ptr<PieceRule>()>> rulesMap = {
        {PieceType::ROOK,   []() { return std::make_unique<RookRule>(); }},
        {PieceType::BISHOP, []() { return std::make_unique<BishopRule>(); }},
        {PieceType::QUEEN,  []() { return std::make_unique<QueenRule>(); }},
        {PieceType::KNIGHT, []() { return std::make_unique<KnightRule>(); }},
        {PieceType::KING,   []() { return std::make_unique<KingRule>(); }},
        {PieceType::PAWN,   []() { return std::make_unique<PawnRule>(); }}
    };
    auto it = rulesMap.find(piece->getType());
    if (it != rulesMap.end()) {
        return it->second(); // מפעילים את הפונקציה ליצירת האובייקט
    }

    return nullptr;

}

// פונקצייה הבודקת האם התנועה חוקית
bool RuleEngine::isLegalMove(std::shared_ptr<Piece> piece, Position from, Position to, const Board& board) const
{
    auto rule = getRule(piece);

    if(!rule) return false;

    if(!rule->canMove(board, from, to)) return false;

    auto destination = board.at(to.getRow(), to.getCol());   // בדיקת משבצת היעד
    //משבצת ריקה
    if(destination == nullptr) return true;
    //מצב אכילה
    return piece->getColor() != destination->getColor();
}

//פונקציה המחזירה את זמן ההליכה של הכלי
int RuleEngine::getTravelTime(std::shared_ptr<Piece> piece, Position from, Position to) const
{
    auto rule = getRule(piece);
    
    if (rule != nullptr)
    {
        return rule->calculateTravelTime(from, to);
    }
    
    return 0;
}

bool RuleEngine::isGameOver(
    const std::shared_ptr<Piece>& capturedPiece) const
{

    if (!capturedPiece)
    {
        return false;
    }

    return capturedPiece->getType() ==
           PieceType::KING;
}
