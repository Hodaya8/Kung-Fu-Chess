#pragma once

#include "position.hpp"

#include <string>

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
    idle,
    move,
    jump,
    long_rest,
    short_rest,
    captured
};

class Piece {
private:
    int id;
    Color color;
    PieceType type;
    Position position;
    PieceState state;

public:
    Piece(int id, Color color, PieceType type, Position position);

    int getId() const;
    Color getColor() const;
    PieceType getType() const;
    Position getPosition() const;
    PieceState getState() const;

    bool isAvailable() const;

    void setPosition(Position p);
    void setState(PieceState newState); 

    std::string toString() const;
};