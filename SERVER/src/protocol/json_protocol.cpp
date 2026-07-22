#include "protocol/json_protocol.hpp"

#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

using Json = nlohmann::json;

namespace
{
    std::string pieceStateToString(PieceState state)
    {
        switch (state)
        {
            case PieceState::idle:
                return "idle";

            case PieceState::move:
                return "move";

            case PieceState::jump:
                return "jump";

            case PieceState::long_rest:
                return "long_rest";

            case PieceState::short_rest:
                return "short_rest";

            case PieceState::captured:
                return "captured";
        }

        throw std::invalid_argument(
            "Unknown piece state."
        );
    }

    std::string colorToString(Color color)
    {
        return color == Color::WHITE
            ? "white"
            : "black";
    }
}

std::string JsonProtocol::getMessageType(
    const std::string& message)
{
    const Json json =
        Json::parse(message);

    return json.at("messageType")
               .get<std::string>();
}

JsonProtocol::LoginRequest
JsonProtocol::parseLoginRequest(
    const std::string& message)
{
    const Json request =
        Json::parse(message);

    const std::string messageType =
        request.at("messageType")
               .get<std::string>();

    if (messageType != "LoginRequest")
    {
        throw std::invalid_argument(
            "Expected LoginRequest."
        );
    }

    const std::string username =
        request.at("username")
               .get<std::string>();

    const std::string password =
        request.at("password")
               .get<std::string>();

    if (username.empty() ||
        password.empty())
    {
        throw std::invalid_argument(
            "Username and password cannot be empty."
        );
    }

    return LoginRequest{
        username,
        password
    };
}

JsonProtocol::ClickRequest
JsonProtocol::parseClickRequest(
    const std::string& message)
{
    const Json request =
        Json::parse(message);

    const std::string messageType =
        request.at("messageType")
               .get<std::string>();

    if (messageType != "ClickRequest")
    {
        throw std::invalid_argument(
            "Expected ClickRequest."
        );
    }

    const int x =
        request.at("x").get<int>();

    const int y =
        request.at("y").get<int>();

    if (x < 0 || y < 0)
    {
        throw std::invalid_argument(
            "Click coordinates cannot be negative."
        );
    }

    const std::string buttonText =
        request.at("button")
               .get<std::string>();

    ClickButton button;

    if (buttonText == "left")
    {
        button = ClickButton::Left;
    }
    else if (buttonText == "right")
    {
        button = ClickButton::Right;
    }
    else
    {
        throw std::invalid_argument(
            "Unknown click button."
        );
    }

    return ClickRequest{
        x,
        y,
        button
    };
}

std::string JsonProtocol::createLoginAcceptedMessage(
    const std::string& username,
    int rating,
    bool registered)
{
    const Json response = {
        {
            "messageType",
            "LoginAccepted"
        },
        {
            "username",
            username
        },
        {
            "rating",
            rating
        },
        {
            "registered",
            registered
        }
    };

    return response.dump();
}

std::string JsonProtocol::createLoginRejectedMessage(
    const std::string& reason)
{
    const Json response = {
        {
            "messageType",
            "LoginRejected"
        },
        {
            "reason",
            reason
        }
    };

    return response.dump();
}

std::string JsonProtocol::createGameStateMessage(
    const GameSnapshot& snapshot,
    bool gameOver,
    int whiteScore,
    int blackScore)
{
    Json pieces = Json::array();

    for (const auto& piece : snapshot.getPieces())
    {
        pieces.push_back({
            {
                "type",
                piece.type
            },
            {
                "state",
                pieceStateToString(
                    piece.state
                )
            },
            {
                "row",
                piece.row
            },
            {
                "col",
                piece.col
            },
            {
                "destinationRow",
                piece.destinationRow
            },
            {
                "destinationCol",
                piece.destinationCol
            },
            {
                "actionStartTime",
                piece.actionStartTime
            },
            {
                "actionEndTime",
                piece.actionEndTime
            },
            {
                "timeRemaining",
                piece.timeRemaining
            },
            {
                "stateDuration",
                piece.stateDuration
            }
        });
    }

    const Json response = {
        {
            "messageType",
            "GameState"
        },
        {
            "currentTime",
            snapshot.getCurrentTime()
        },
        {
            "gameOver",
            gameOver
        },
        {
            "whiteScore",
            whiteScore
        },
        {
            "blackScore",
            blackScore
        },
        {
            "pieces",
            pieces
        }
    };

    return response.dump();
}

std::string JsonProtocol::createSelectionStateMessage(
    const std::optional<Position>& selectedPosition)
{
    Json response = {
        {
            "messageType",
            "SelectionState"
        },
        {
            "hasSelection",
            selectedPosition.has_value()
        }
    };

    if (selectedPosition.has_value())
    {
        response["row"] =
            selectedPosition->getRow();

        response["col"] =
            selectedPosition->getCol();
    }

    return response.dump();
}

std::string JsonProtocol::createPlayerAssignedMessage(
    Color playerColor)
{
    const Json response = {
        {
            "messageType",
            "PlayerAssigned"
        },
        {
            "side",
            colorToString(
                playerColor
            )
        }
    };

    return response.dump();
}

std::string JsonProtocol::createGameFullMessage()
{
    const Json response = {
        {
            "messageType",
            "ConnectionRejected"
        },
        {
            "reason",
            "GameFull"
        }
    };

    return response.dump();
}