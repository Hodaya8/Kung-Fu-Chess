#include "render/render_model.hpp"

#include <algorithm>

namespace
{
    constexpr int IDLE_ANIMATION_DURATION_MS = 500;

    double calculateActionProgress(
        int currentTime,
        int startTime,
        int endTime)
    {
        if (endTime <= startTime)
        {
            return 1.0;
        }

        const int elapsed =
            currentTime - startTime;

        const int duration =
            endTime - startTime;

        const double progress =
            static_cast<double>(elapsed) /
            static_cast<double>(duration);

        return std::clamp(
            progress,
            0.0,
            1.0
        );
    }

    double calculateRestProgress(
        int timeRemaining,
        int duration)
    {
        if (duration <= 0 ||
            timeRemaining < 0)
        {
            return 0.0;
        }

        const double progress =
            1.0 -
            static_cast<double>(timeRemaining) /
            static_cast<double>(duration);

        return std::clamp(
            progress,
            0.0,
            1.0
        );
    }
}

RenderModel::RenderModel(
    const std::vector<PieceRenderData>& pieces)
    : pieces(pieces)
{
}

const std::vector<PieceRenderData>&
RenderModel::getPieces() const
{
    return pieces;
}

RenderModel buildRenderModel(
    const GameSnapshot& snapshot)
{
    std::vector<PieceRenderData> renderPieces;

    const int currentTime =
        snapshot.getCurrentTime();

    for (const auto& piece :
         snapshot.getPieces())
    {
        PieceRenderData renderPiece;

        renderPiece.type = piece.type;
        renderPiece.state = piece.state;
        renderPiece.timeRemaining =
            piece.timeRemaining;

        renderPiece.row =
            static_cast<double>(piece.row);

        renderPiece.col =
            static_cast<double>(piece.col);

        switch (piece.state)
        {
            case PieceState::idle:
            {
                renderPiece.animationProgress =
                    static_cast<double>(
                        currentTime %
                        IDLE_ANIMATION_DURATION_MS
                    ) /
                    IDLE_ANIMATION_DURATION_MS;

                break;
            }

            case PieceState::move:
            {
                const double progress =
                    calculateActionProgress(
                        currentTime,
                        piece.actionStartTime,
                        piece.actionEndTime
                    );

                renderPiece.animationProgress =
                    progress;

                renderPiece.row =
                    piece.row +
                    (piece.destinationRow -
                     piece.row) *
                    progress;

                renderPiece.col =
                    piece.col +
                    (piece.destinationCol -
                     piece.col) *
                    progress;

                break;
            }

            case PieceState::jump:
            {
                renderPiece.animationProgress =
                    calculateActionProgress(
                        currentTime,
                        piece.actionStartTime,
                        piece.actionEndTime
                    );

                break;
            }

            case PieceState::short_rest:
            case PieceState::long_rest:
            {
                renderPiece.animationProgress =
                    calculateRestProgress(
                        piece.timeRemaining,
                        piece.stateDuration
                    );

                break;
            }

            case PieceState::captured:
            {
                renderPiece.animationProgress = 0.0;
                break;
            }
        }

        renderPieces.push_back(renderPiece);
    }

    return RenderModel(renderPieces);
}