#include "model/piece.hpp"
#include <unordered_map>

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
state(PieceState::idle)
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

void Piece::setState(PieceState newState) 
{
    state = newState;
}

bool Piece::isAvailable() const {
    return state == PieceState::idle;
}

std::string Piece::toString() const {
    static const std::unordered_map<PieceType, char> typeMap = {
        {PieceType::KING,   'K'},
        {PieceType::QUEEN,  'Q'},
        {PieceType::ROOK,   'R'},
        {PieceType::BISHOP, 'B'},
        {PieceType::KNIGHT, 'N'},
        {PieceType::PAWN,   'P'}
    };

    std::string s = "";
    s += (color == Color::WHITE) ? 'w' : 'b';
    
    s += typeMap.at(type);
    
    return s;
}