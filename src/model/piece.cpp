#include "model/piece.hpp"


Piece::Piece(
    int id,
    Color color,
    PieceType type,
    Position position
)
:
id(id),
color(color),
type(type),
position(position),
state(PieceState::IDLE)
{
}


int Piece::getId() const
{
    return id;
}


Color Piece::getColor() const
{
    return color;
}


PieceType Piece::getType() const
{
    return type;
}


Position Piece::getPosition() const
{
    return position;
}


PieceState Piece::getState() const
{
    return state;
}


void Piece::setPosition(Position p)
{
    position = p;
}