#include "rules/QueenRule.hpp"
#include "rules/RookRule.hpp"
#include "rules/BishopRule.hpp"


bool QueenRule::canMove(
    const Board& board,
    Position from,
    Position to
) const
{
    RookRule rook;
    BishopRule bishop;

    return rook.canMove(board, from, to) || bishop.canMove(board, from, to);

}