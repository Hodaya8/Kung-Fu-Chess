#include "scoring/score_service.hpp"

int ScoreService::pointsFor(PieceType pieceType) {
    switch (pieceType) {
        case PieceType::PAWN:
            return 1;

        case PieceType::KNIGHT:
        case PieceType::BISHOP:
            return 3;

        case PieceType::ROOK:
            return 5;

        case PieceType::QUEEN:
            return 9;

        case PieceType::KING:
            return 0;
    }

    return 0;
}

void ScoreService::handlePieceRemoved(const PieceRemovedInfo& removedPiece) {
    const int points = pointsFor(removedPiece.removedType);

    if (removedPiece.scoringColor == Color::WHITE) {
        whiteScore += points;
    }
    else if (removedPiece.scoringColor == Color::BLACK) {
        blackScore += points;
    }
}

int ScoreService::getWhiteScore() const {
    return whiteScore;
}

int ScoreService::getBlackScore() const {
    return blackScore;
}