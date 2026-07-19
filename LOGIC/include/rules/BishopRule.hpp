#pragma once

#include "PieceRule.hpp"


class BishopRule : public PieceRule
{

public:

    bool canMove(
        const Board& board,
        Position from,
        Position to
    ) const override;

};