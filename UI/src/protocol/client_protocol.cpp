#include "protocol/client_protocol.hpp"

#include <nlohmann/json.hpp>

using Json = nlohmann::json;

std::string
ClientProtocol::createLoginRequestMessage(
    const std::string& username,
    const std::string& password)
{
    const Json request = {
        {
            "messageType",
            "LoginRequest"
        },
        {
            "username",
            username
        },
        {
            "password",
            password
        }
    };

    return request.dump();
}

std::string
ClientProtocol::createMoveRequestMessage(
    const Position& source,
    const Position& destination)
{
    const Json request = {
        {
            "messageType",
            "MoveRequest"
        },
        {
            "sourceRow",
            source.getRow()
        },
        {
            "sourceCol",
            source.getCol()
        },
        {
            "destinationRow",
            destination.getRow()
        },
        {
            "destinationCol",
            destination.getCol()
        }
    };

    return request.dump();
}

std::string
ClientProtocol::createJumpRequestMessage(
    const Position& cell)
{
    const Json request = {
        {
            "messageType",
            "JumpRequest"
        },
        {
            "row",
            cell.getRow()
        },
        {
            "col",
            cell.getCol()
        }
    };

    return request.dump();
}