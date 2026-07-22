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

    struct LoginRequest
    {
        std::string username;
        std::string password;
    };

    std::string getMessageType(
        const std::string& message
    );

    LoginRequest parseLoginRequest(
        const std::string& message
    );

    ClickRequest parseClickRequest(
        const std::string& message
    );

    std::string createLoginAcceptedMessage(
        const std::string& username,
        int rating,
        bool registered
    );

    std::string createLoginRejectedMessage(
        const std::string& reason
    );

    std::string createGameStateMessage(
        const GameSnapshot& snapshot,
        bool gameOver,
        int whiteScore,
        int blackScore
    );

    std::string createSelectionStateMessage(
        const std::optional<Position>& selectedPosition
    );

    std::string createPlayerAssignedMessage(
        Color playerColor
    );

    std::string createGameFullMessage();
}