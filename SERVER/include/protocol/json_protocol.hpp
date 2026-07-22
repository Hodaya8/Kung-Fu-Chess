#pragma once

#include <optional>
#include <string>

#include "model/game_snapshot.hpp"
#include "model/position.hpp"

namespace JsonProtocol
{
    enum class ClickButton
    {
        Left,
        Right
    };

    struct ClickRequest
    {
        int x;
        int y;
        ClickButton button;
    };

    // GameSnapshot למחרוזת JSON
    std::string createGameStateMessage(
        const GameSnapshot& snapshot,
        bool gameOver
    );

    // מצב בחירה למחרוזת JSON
    std::string createSelectionStateMessage(
        const std::optional<Position>&
            selectedPosition
    );

    // מחרוזת JSON לבקשת קליק
    ClickRequest parseClickRequest(
        const std::string& message
    );
}