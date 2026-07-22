#include "rating/elo_service.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr double RATING_SCALE = 400.0;

    double calculateExpectedScore(
        int playerRating,
        int opponentRating)
    {
        const double ratingDifference =
            static_cast<double>(
                opponentRating -
                playerRating
            );

        return 1.0 /
            (
                1.0 +
                std::pow(
                    10.0,
                    ratingDifference /
                        RATING_SCALE
                )
            );
    }
}

EloResult EloService::calculateRatings(
    int whiteRating,
    int blackRating,
    Color winnerColor) const
{
    const double expectedWhite =
        calculateExpectedScore(
            whiteRating,
            blackRating
        );

    const double expectedBlack =
        calculateExpectedScore(
            blackRating,
            whiteRating
        );

    const double actualWhite =
        winnerColor == Color::WHITE
            ? 1.0
            : 0.0;

    const double actualBlack =
        winnerColor == Color::BLACK
            ? 1.0
            : 0.0;

    const int newWhiteRating =
        static_cast<int>(
            std::lround(
                whiteRating +
                K_FACTOR *
                (
                    actualWhite -
                    expectedWhite
                )
            )
        );

    const int newBlackRating =
        static_cast<int>(
            std::lround(
                blackRating +
                K_FACTOR *
                (
                    actualBlack -
                    expectedBlack
                )
            )
        );

    return EloResult{
        std::max(
            0,
            newWhiteRating
        ),
        std::max(
            0,
            newBlackRating
        )
    };
}