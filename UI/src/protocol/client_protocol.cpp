#include "protocol/client_protocol.hpp"

#include <string>

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
ClientProtocol::createClickRequestMessage(
    const MouseClick& click)
{
    const std::string button =
        click.button == MouseButton::Left
            ? "left"
            : "right";

    const Json request = {
        {
            "messageType",
            "ClickRequest"
        },
        {
            "x",
            click.x
        },
        {
            "y",
            click.y
        },
        {
            "button",
            button
        }
    };

    return request.dump();
}