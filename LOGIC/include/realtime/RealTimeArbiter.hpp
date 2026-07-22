#pragma once

#include <memory>
#include <vector>

#include "realtime/Motion.hpp"
#include "model/board.hpp"
#include "model/piece_removed_info.hpp"


struct ActiveJump
{
    Position cell;
    int startTime;
    int arrivalTime;
};

class RealTimeArbiter
{
private:
    std::vector<Motion> activeMotions;
    std::vector<ActiveJump> activeJumps;

    int currentTime = 0;

    std::shared_ptr<Piece> handlePromotion(
        Board& board,
        Position destination,
        std::shared_ptr<Piece> piece
    );

public:
    void addMotion(const Motion& motion);
    bool has_active_motion() const;

    void addJump(Position cell, int duration = 1000);
    bool isPieceJumping(Position cell) const;
    bool isPieceMoving(Position cell) const;

    std::vector<PieceRemovedInfo> advance_time(
        int milliseconds,
        Board& board
    );

    int getCurrentTime() const
    {
        return currentTime;
    }

    const std::vector<Motion>& getActiveMotions() const
    {
        return activeMotions;
    }

    const std::vector<ActiveJump>& getActiveJumps() const
    {
        return activeJumps;
    }
};