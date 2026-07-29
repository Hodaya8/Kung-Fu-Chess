#include "protocol/json_protocol.hpp"

#include <map>
#include <stdexcept>
#include <string>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

namespace
{
    constexpr int BOARD_SIZE = 8;

    //Converts internal C++ PieceState enum to a string for JSON format
    std::string pieceStateToString(PieceState state)
    {
        static const std::map<PieceState, std::string> stateNames = {
            {PieceState::idle, "idle"},
            {PieceState::move, "move"},
            {PieceState::jump, "jump"},
            {PieceState::long_rest, "long_rest"},
            {PieceState::short_rest, "short_rest"},
            {PieceState::captured, "captured"}
        };

        const auto stateIterator = stateNames.find(state);
        if (stateIterator == stateNames.end())
        {
            throw std::invalid_argument("Unknown piece state.");
        }

        return stateIterator->second;
    }

    //Converts Color enum to "white" or "black"
    std::string colorToString(Color color)
    {
        return color == Color::WHITE ? "white" : "black";
    }

    //Safely reads row and column numbers from JSON and validates to position
    Position readPosition(const Json& json, const std::string& rowField, const std::string& columnField)
    {
        const int row = json.at(rowField).get<int>();
        const int column = json.at(columnField).get<int>();

        if (row < 0 || row >= BOARD_SIZE || column < 0 || column >= BOARD_SIZE)
        {
            throw std::invalid_argument("Position is outside the board.");
        }

        return Position(row, column);
    }
}

std::string JsonProtocol::getMessageType(const std::string& message)
{
    const Json json = Json::parse(message);
    return json.at("messageType").get<std::string>();
}

JsonProtocol::LoginRequest JsonProtocol::parseLoginRequest(const std::string& message)
{
    const Json request = Json::parse(message);
    const std::string messageType = request.at("messageType").get<std::string>();

    if (messageType != "LoginRequest")
    {
        throw std::invalid_argument("Expected LoginRequest.");
    }

    const std::string username = request.at("username").get<std::string>();
    const std::string password = request.at("password").get<std::string>();

    if (username.empty() || password.empty())
    {
        throw std::invalid_argument("Username and password cannot be empty.");
    }

    return LoginRequest{username, password};
}

JsonProtocol::MoveRequest JsonProtocol::parseMoveRequest(const std::string& message)
{
    const Json request = Json::parse(message);
    const std::string messageType = request.at("messageType").get<std::string>();

    if (messageType != "MoveRequest")
    {
        throw std::invalid_argument("Expected MoveRequest.");
    }

    const Position source = readPosition(request, "sourceRow", "sourceCol");
    const Position destination = readPosition(request, "destinationRow", "destinationCol");

    return MoveRequest{source, destination};
}

JsonProtocol::JumpRequest JsonProtocol::parseJumpRequest(const std::string& message)
{
    const Json request = Json::parse(message);
    const std::string messageType = request.at("messageType").get<std::string>();

    if (messageType != "JumpRequest")
    {
        throw std::invalid_argument("Expected JumpRequest.");
    }

    const Position cell = readPosition(request, "row", "col");
    return JumpRequest{cell};
}

std::string JsonProtocol::createLoginAcceptedMessage(const std::string& username, int rating, bool registered)
{
    const Json response = {
        {"messageType", "LoginAccepted"},
        {"username", username},
        {"rating", rating},
        {"registered", registered}
    };

    return response.dump();
}

std::string JsonProtocol::createLoginRejectedMessage(const std::string& reason)
{
    const Json response = {
        {"messageType", "LoginRejected"},
        {"reason", reason}
    };

    return response.dump();
}

std::string JsonProtocol::createWaitingForMatchMessage()
{
    const Json response = {
        {"messageType", "WaitingForMatch"}
    };

    return response.dump();
}

std::string JsonProtocol::createMatchFoundMessage(int roomId, Color playerColor)
{
    const Json response = {
        {"messageType", "MatchFound"},
        {"roomId", roomId},
        {"side", colorToString(playerColor)}
    };

    return response.dump();
}

//updating game each second-for real time game display
std::string JsonProtocol::createGameStateMessage(
    const GameSnapshot& snapshot,
    bool gameOver,
    int whiteScore,
    int blackScore)
{
    //for pieces
    Json pieces = Json::array();

    for (const auto& piece : snapshot.getPieces())
    {
        pieces.push_back({
            {"type", piece.type},
            {"state", pieceStateToString(piece.state)},
            {"row", piece.row},
            {"col", piece.col},
            {"destinationRow", piece.destinationRow},
            {"destinationCol", piece.destinationCol},
            {"actionStartTime", piece.actionStartTime},
            {"actionEndTime", piece.actionEndTime},
            {"timeRemaining", piece.timeRemaining},
            {"stateDuration", piece.stateDuration}
        });
    }

    const Json response = {
        {"messageType", "GameState"},
        {"currentTime", snapshot.getCurrentTime()},
        {"gameOver", gameOver},
        {"whiteScore", whiteScore},
        {"blackScore", blackScore},
        {"pieces", pieces}
    };

    return response.dump();
}