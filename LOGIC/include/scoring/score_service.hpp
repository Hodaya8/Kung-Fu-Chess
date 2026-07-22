#pragma once

#include "model/piece_removed_info.hpp"

class ScoreService {
private:
    int whiteScore = 0;
    int blackScore = 0;

    static int pointsFor(PieceType pieceType);

public:
    void handlePieceRemoved(const PieceRemovedInfo& removedPiece);

    int getWhiteScore() const;
    int getBlackScore() const;
};