#pragma once

#include "model/piece.hpp"
#include "model/Position.hpp"
#include "rules/PieceRule.hpp"

#include <string>
#include <memory>


class RuleEngine
{

private:
// פונקציה המחזירה את שם הכלי  
    std::unique_ptr<PieceRule> getRule(
        std::shared_ptr<Piece> piece
    ) const;


public:
    // האם זו תנועה חוקית
    bool isLegalMove(
        std::shared_ptr<Piece> piece,
        Position from,
        Position to,
        const Board& board
    ) const;

    int getTravelTime(
    std::shared_ptr<Piece> piece,
    Position from,
    Position to
    ) const;

    bool isGameOver(
        const std::shared_ptr<Piece>& capturedPiece
    ) const;


};