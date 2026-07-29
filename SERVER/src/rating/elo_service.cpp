#include "include/rating/elo_service.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr double RATING_SCALE = 400.0;

    // Calculates the win probability (a value between 0.0 and 1.0) for a player
    double calculateExpectedScore(int playerRating, int opponentRating)
    {
        const double ratingDifference = static_cast<double>(opponentRating - playerRating);
        
        // Standard FIDE ELO formula: 1 / (1 + 10^(diff / 400))
        return 1.0 / (1.0 + std::pow(10.0, ratingDifference / RATING_SCALE));
    }
}

namespace EloService
{
    EloResult calculateRatings(int whiteRating, int blackRating, GameResult result)
    {
        // Calculate the statistical win probability for both players
        const double expectedWhite = calculateExpectedScore(whiteRating, blackRating);
        const double expectedBlack = calculateExpectedScore(blackRating, whiteRating);

        double actualWhite = 0.0;
        double actualBlack = 0.0;

        switch (result)
        {
            case GameResult::WHITE_WIN:
                actualWhite = 1.0;
                actualBlack = 0.0;
                break;
            case GameResult::BLACK_WIN:
                actualWhite = 0.0;
                actualBlack = 1.0;
                break;
            case GameResult::DRAW:
                actualWhite = 0.5;
                actualBlack = 0.5;
                break;
        }

        //Calculate new ratings: New = Old + K * (Actual - Expected)
        const int newWhiteRating = static_cast<int>(
            std::lround(whiteRating + K_FACTOR * (actualWhite - expectedWhite))
        );

        const int newBlackRating = static_cast<int>(
            std::lround(blackRating + K_FACTOR * (actualBlack - expectedBlack))
        );

        //Return the updated ratings, ensuring neither player drops below 0
        return EloResult{
            std::max(0, newWhiteRating),
            std::max(0, newBlackRating)
        };
    }
}