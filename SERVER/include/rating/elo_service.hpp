#pragma once

#include "model/piece.hpp"

struct EloResult
{
    int whiteRating;
    int blackRating;
};

class EloService
{
private:
    static constexpr int K_FACTOR = 32;

public:
    EloResult calculateRatings(
        int whiteRating,
        int blackRating,
        Color winnerColor
    ) const;
};