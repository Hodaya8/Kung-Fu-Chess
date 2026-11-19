#pragma once

#include <string>

// The result of trying to log in through the API Gateway.
struct AuthResult
{
    bool success;
    std::string token;
    std::string errorMessage;
};

// Talks to the Python API Gateway over HTTP.
// Logs the user in and returns a session token for the game server.
// If the user does not exist yet, registers them first.
class AuthClient
{
public:
    AuthClient(
        std::string host,
        std::string port
    );

    AuthResult loginOrRegister(
        const std::string& username,
        const std::string& password
    ) const;

private:
    struct HttpResponse
    {
        int statusCode;
        std::string body;
    };

    HttpResponse post(
        const std::string& path,
        const std::string& jsonBody
    ) const;

    std::string host;
    std::string port;
};
