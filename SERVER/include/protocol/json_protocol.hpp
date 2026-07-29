#pragma once

#include <string>

#include "model/game_snapshot.hpp"
#include "model/piece.hpp"
#include "model/position.hpp"

namespace JsonProtocol
{
    // Structures representing parsed client requests
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

    // Extracts the "messageType" string from a raw JSON message
    std::string getMessageType(const std::string& message);

    // Parsing functions (JSON string -> C++ Structs)
    LoginRequest parseLoginRequest(const std::string& message);
    MoveRequest parseMoveRequest(const std::string& message);
    JumpRequest parseJumpRequest(const std::string& message);

    // Serialization functions (C++ Data -> JSON string responses)
    std::string createLoginAcceptedMessage(const std::string& username, int rating, bool registered);
    std::string createLoginRejectedMessage(const std::string& reason);
    std::string createWaitingForMatchMessage();
    std::string createMatchFoundMessage(int roomId, Color playerColor);
    std::string createGameStateMessage(
        const GameSnapshot& snapshot,
        bool gameOver,
        int whiteScore,
        int blackScore
    );
}