#pragma once

#include <string>
#include <vector>

#include "model/game_snapshot.hpp"

struct PieceRenderData
{
    std::string type;

    double row = 0.0;
    double col = 0.0;

    PieceState state = PieceState::idle;
    double animationProgress = 0.0;

    int timeRemaining = -1;
};

class RenderModel
{
private:
    std::vector<PieceRenderData> pieces;

public:
    explicit RenderModel(
        const std::vector<PieceRenderData>& pieces
    );

    const std::vector<PieceRenderData>&
    getPieces() const;
};

RenderModel buildRenderModel(
    const GameSnapshot& snapshot
);