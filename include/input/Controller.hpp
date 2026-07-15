#pragma once

#include "model/Board.hpp"
#include "input/BoardMapper.hpp"
#include "engine/GameEngine.hpp"

class Controller
{

private:

    Board& board;
    BoardMapper& mapper;
    GameEngine& engine;
    bool hasSelection;
    Position selected;


public:

    Controller(
        Board& board,
        BoardMapper& mapper,
        GameEngine& engine
    );


    void click(int x,int y);
    void jump(int x, int y);

    bool isSelected() const;

    Position getSelected() const;
};