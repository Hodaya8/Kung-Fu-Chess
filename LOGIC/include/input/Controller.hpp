#pragma once

#include "model/Board.hpp"
#include "input/BoardMapper.hpp"
#include "engine/GameEngine.hpp"
#include "model/piece.hpp"

class Controller
{

private:

    Board& board;
    BoardMapper& mapper;
    GameEngine& engine;
    bool hasSelection;
    Position selected;
    Color playerColor;

    bool isPlayerPiece(
        const Position& cell
    ) const;

    void selectCell(
        const Position& cell
    );

public:

    Controller(
        Board& board,
        BoardMapper& mapper,
        GameEngine& engine,
        Color playerColor
    );


    void click(int x,int y);
    void jump(int x, int y);

    bool isSelected() const;

    Position getSelected() const;
};