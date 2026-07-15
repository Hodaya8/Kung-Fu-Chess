#pragma once

#include "rules/PieceRule.hpp"


class PawnRule : public PieceRule
{

public:

    bool canMove(
        const Board& board,
        Position from,
        Position to
    ) const override;
    

};