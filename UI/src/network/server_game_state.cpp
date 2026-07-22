#include "network/server_game_state.hpp"

#include <stdexcept>
#include <vector>

#include <nlohmann/json.hpp>

using Json = nlohmann::json;

namespace
{
    // ממירה טקסט שהתקבל ב-JSON ל-enum של C++
    PieceState pieceStateFromString(
        const std::string& stateText)
    {
        if (stateText == "idle")
        {
            return PieceState::idle;
        }

        if (stateText == "move")
        {
            return PieceState::move;
        }

        if (stateText == "jump")
        {
            return PieceState::jump;
        }

        if (stateText == "long_rest")
        {
            return PieceState::long_rest;
        }

        if (stateText == "short_rest")
        {
            return PieceState::short_rest;
        }

        if (stateText == "captured")
        {
            return PieceState::captured;
        }

        throw std::invalid_argument(
            "Unknown piece state: " +
            stateText
        );
    }
}

std::optional<ServerGameState>
parseServerGameState(
    const std::string& message)
{
    // שלב 1: המרת המחרוזת לאובייקט JSON
    const Json json =
        Json::parse(message);

    const std::string messageType =
        json.value(
            "messageType",
            std::string{}
        );

    // בעתיד ייתכן שיהיו סוגי הודעות נוספים
    if (messageType != "GameState")
    {
        return std::nullopt;
    }

    const Json& piecesJson =
        json.at("pieces");

    if (!piecesJson.is_array())
    {
        throw std::invalid_argument(
            "GameState pieces must be an array."
        );
    }

    std::vector<PieceSnapshot> pieces;

    // שלב 2: המרת כל כלי מה-JSON ל-PieceSnapshot
    for (const auto& pieceJson :
         piecesJson)
    {
        PieceSnapshot piece;

        piece.type =
            pieceJson.at("type")
                     .get<std::string>();

        piece.state =
            pieceStateFromString(
                pieceJson.at("state")
                         .get<std::string>()
            );

        piece.row =
            pieceJson.at("row")
                     .get<int>();

        piece.col =
            pieceJson.at("col")
                     .get<int>();

        piece.destinationRow =
            pieceJson.at("destinationRow")
                     .get<int>();

        piece.destinationCol =
            pieceJson.at("destinationCol")
                     .get<int>();

        piece.actionStartTime =
            pieceJson.at("actionStartTime")
                     .get<int>();

        piece.actionEndTime =
            pieceJson.at("actionEndTime")
                     .get<int>();

        piece.timeRemaining =
            pieceJson.at("timeRemaining")
                     .get<int>();

        piece.stateDuration =
            pieceJson.at("stateDuration")
                     .get<int>();

        pieces.push_back(piece);
    }

    const int currentTime =
        json.at("currentTime")
            .get<int>();

    const bool gameOver =
        json.at("gameOver")
            .get<bool>();

    // שלב 3: יצירת GameSnapshot חדש בצד ה-UI
    return ServerGameState{
        GameSnapshot(
            pieces,
            currentTime
        ),
        gameOver
    };
}