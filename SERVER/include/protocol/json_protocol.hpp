#pragma once

#include <optional>
#include <string>

#include "model/game_snapshot.hpp"
#include "model/piece.hpp"
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

    std::string createGameStateMessage(
        const GameSnapshot& snapshot,
        bool gameOver
    );

    std::string createSelectionStateMessage(
        const std::optional<Position>&
            selectedPosition
    );

    std::string createPlayerAssignedMessage(
        Color playerColor
    );

    std::string createGameFullMessage();

    ClickRequest parseClickRequest(
        const std::string& message
    );
}