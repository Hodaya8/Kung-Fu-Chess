#pragma once

#include <memory>

#include "model/board.hpp"
#include "model/piece.hpp"
#include "model/position.hpp"
#include "rules/PieceRule.hpp"

class RuleEngine
{
private:
    // יצירת חוק התנועה המתאים לסוג הכלי
    std::unique_ptr<PieceRule> getRule(
        std::shared_ptr<Piece> piece
    ) const;

public:
    // בדיקה האם התנועה חוקית
    bool isLegalMove(
        std::shared_ptr<Piece> piece,
        Position from,
        Position to,
        const Board& board
    ) const;

    // החזרת זמן התנועה של הכלי
    int getTravelTime(
        std::shared_ptr<Piece> piece,
        Position from,
        Position to
    ) const;

    // בדיקה האם יציאת הכלי מסיימת את המשחק
    bool isGameOver(
        PieceType removedPieceType
    ) const;
};