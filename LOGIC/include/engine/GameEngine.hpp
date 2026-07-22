#pragma once

#include <map>
#include <memory>
#include <vector>

#include "model/board.hpp"
#include "model/position.hpp"
#include "model/game_snapshot.hpp"
#include "model/piece_removed_info.hpp"
#include "rules/RuleEngine.hpp"
#include "realtime/RealTimeArbiter.hpp"

class GameEngine
{
private:
    Board& board;
    RuleEngine ruleEngine;
    RealTimeArbiter arbiter;
    bool game_over;

    mutable std::map<int, int> restStartTimes;

    void addStaticSnapshots(std::vector<PieceSnapshot>& snapshots) const;
    void addJumpingSnapshots(std::vector<PieceSnapshot>& snapshots) const;
    void addMovingSnapshots(std::vector<PieceSnapshot>& snapshots) const;
    void handleRestState(PieceSnapshot& snapshot, std::shared_ptr<Piece> piece) const;

public:
    explicit GameEngine(Board& board);

    void requestMove(Position source, Position destination);
    void requestJump(Position cell);

    std::vector<PieceRemovedInfo> wait(int milliseconds);

    GameSnapshot getSnapshot() const;

    bool isGameOver() const {
        return game_over;
    }
};