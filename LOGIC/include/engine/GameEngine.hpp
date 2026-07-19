#pragma once

#include "board.hpp"
#include "position.hpp"
#include "rules/RuleEngine.hpp"
#include "RealTimeArbiter.hpp"
#include "Motion.hpp"
#include "model/game_snapshot.hpp" 
#include <map>

class GameEngine
{
private:
    Board& board;
    RuleEngine ruleEngine;
    RealTimeArbiter arbiter;
    bool game_over;
    mutable std::map<int, int> restStartTimes;   // תחילת זמ המנוחה של כל כלי
     //פונקציות עזר לgetSnapshot

    void addStaticSnapshots(std::vector<PieceSnapshot>& snapshots) const;
    void addJumpingSnapshots(std::vector<PieceSnapshot>& snapshots) const; // הוספנו את זה
    void addMovingSnapshots(std::vector<PieceSnapshot>& snapshots) const;
    void handleRestState(PieceSnapshot& snap, std::shared_ptr<Piece> piece) const;

public:
    GameEngine(Board& board);

    void requestMove(Position source, Position destination);
    void requestJump(Position cell); 

    void wait(int ms);
    void handlePromotion();

    GameSnapshot getSnapshot() const; 

    bool hasActiveMotion() const;
    bool isGameOver() const { return game_over; }

};