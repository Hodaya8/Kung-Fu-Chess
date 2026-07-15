#pragma once

#include "model/board.hpp"
#include "model/Position.hpp"
#include "rules/RuleEngine.hpp"
#include "realtime/RealTimeArbiter.hpp"
#include "realtime/Motion.hpp"

class GameEngine
{

private:

    Board& board;
    //לבדיקת חוקיות תנועה
    RuleEngine ruleEngine;
    RealTimeArbiter arbiter;
    bool game_over;

public:

    GameEngine(Board& board);

    void requestMove(Position source, Position destination);
    void requestJump(Position cell); 

    void wait(int ms);
    void handlePromotion();


};