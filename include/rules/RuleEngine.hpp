#pragma once

#include "model/Position.hpp"
#include "rules/PieceRule.hpp"

#include <string>
#include <memory>


class RuleEngine
{

private:
// פונקציה המחזירה את שם הכלי  
    std::unique_ptr<PieceRule> getRule(
        const std::string& piece
    ) const;


public:
    // האם זו תנועה חוקית
    bool isLegalMove(
        const std::string& piece,
        Position from,
        Position to,
        const Board& board
    ) const;

    int getTravelTime(
    const std::string& piece,
    Position from,
    Position to
    ) const;

    

};