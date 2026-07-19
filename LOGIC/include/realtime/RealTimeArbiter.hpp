#pragma once
#include "Motion.hpp"
#include "model/board.hpp"
#include <vector>
#include <string>

struct ActiveJump {
    Position cell;
    int arrivalTime;
};

class RealTimeArbiter {
private:
    std::vector<Motion> activeMotions; // יכיל לכל היותר תנועה אחת
    std::vector<ActiveJump> activeJumps; // מעקב אחר קפיצות פעילות באוויר
    int currentTime = 0;
    // פונקציה המחזירה את הכלי המעודכן- מוכתר או שלא
    std::shared_ptr<Piece> handlePromotion(Board& board, Position dest, std::shared_ptr<Piece> piece);

public:
    void addMotion(const Motion& motion);
    bool has_active_motion() const; // המתודה החדשה שלנו
    // מתודות לניהול קפיצה
    void addJump(Position cell, int duration = 1000);
    bool isPieceJumping(Position cell) const;
    bool isPieceMoving(Position cell) const;
    std::shared_ptr<Piece> advance_time(int ms, Board& board);
    int getCurrentTime() const { return currentTime; };
    const std::vector<Motion>& getActiveMotions() const { return activeMotions; }
    const std::vector<ActiveJump>& getActiveJumps() const { return activeJumps; }
};