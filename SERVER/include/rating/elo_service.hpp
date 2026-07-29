#pragma once

#include "model/piece.hpp"

// Represents all possible game outcomes
enum class GameResult
{
    WHITE_WIN,
    BLACK_WIN,
    DRAW
};

// Holds the updated ratings after calculation
struct EloResult
{
    int whiteRating = 0;
    int blackRating = 0;
};

namespace EloService
{
    // K-factor determines how drastically a rating can change in a single game
    inline constexpr int K_FACTOR = 32;

    // Calculates new ELO ratings based on current ratings and game outcome
    EloResult calculateRatings(
        int whiteRating,
        int blackRating,
        GameResult result
    );
}