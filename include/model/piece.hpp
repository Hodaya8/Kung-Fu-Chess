#pragma once

#include "position.hpp"


enum class Color
{
    WHITE,
    BLACK
};


enum class PieceType
{
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN
};


enum class PieceState
{
    IDLE,
    MOVING,
    CAPTURED
};



class Piece
{

private:

    int id;

    Color color;

    PieceType type;

    Position position;

    PieceState state;


public:

    Piece(
        int id,
        Color color,
        PieceType type,
        Position position
    );


    int getId() const;

    Color getColor() const;

    PieceType getType() const;

    Position getPosition() const;


    PieceState getState() const;


    void setPosition(Position p);

};