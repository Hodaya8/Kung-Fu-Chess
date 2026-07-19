#include "game_snapshot.hpp"

GameSnapshot::GameSnapshot(
    const std::vector<PieceSnapshot>& pieces
)
:
pieces(pieces)
{

}

const std::vector<PieceSnapshot>& 
GameSnapshot::getPieces() const
{
    return pieces;
}