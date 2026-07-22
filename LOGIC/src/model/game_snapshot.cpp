// #include "game_snapshot.hpp"

// GameSnapshot::GameSnapshot(
//     const std::vector<PieceSnapshot>& pieces
// )
// :
// pieces(pieces)
// {

// }

// const std::vector<PieceSnapshot>& 
// GameSnapshot::getPieces() const
// {
//     return pieces;
// }


#include "model/game_snapshot.hpp"

GameSnapshot::GameSnapshot(
    const std::vector<PieceSnapshot>& pieces,
    int currentTime)
    : pieces(pieces),
      currentTime(currentTime)
{
}

const std::vector<PieceSnapshot>&
GameSnapshot::getPieces() const
{
    return pieces;
}

int GameSnapshot::getCurrentTime() const
{
    return currentTime;
}