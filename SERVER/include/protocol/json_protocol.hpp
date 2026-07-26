#pragma once

#include <string>

#include "model/game_snapshot.hpp"
#include "model/piece.hpp"
#include "model/position.hpp"

namespace JsonProtocol
{
    struct LoginRequest
    {
        std::string username;
        std::string password;
    };

    struct MoveRequest
    {
        Position source;
        Position destination;
    };

    struct JumpRequest
    {
        Position cell;
    };

    std::string getMessageType(
        const std::string& message
    );

    LoginRequest parseLoginRequest(
        const std::string& message
    );

    MoveRequest parseMoveRequest(
        const std::string& message
    );

    JumpRequest parseJumpRequest(
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

    std::string createPlayerAssignedMessage(
        Color playerColor
    );

    std::string createGameFullMessage();
}