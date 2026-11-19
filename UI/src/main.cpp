#include <exception>
#include <iostream>
#include <string>

#include "application/ui_application.hpp"
#include "network/auth_client.hpp"

namespace
{
    const std::string SERVER_URI =
        "ws://127.0.0.1:9000";

    const std::string AUTH_HOST =
        "127.0.0.1";

    const std::string AUTH_PORT =
        "8000";

    const std::string ASSETS_DIRECTORY =
        "asset";
}

int main()
{
    std::string username;
    std::string password;

    std::cout
        << "===============================\n"
        << "   Welcome to KungFu Chess!    \n"
        << "===============================\n";

    std::cout
        << "Username: ";

    std::cin >> username;

    std::cout
        << "Password: ";

    std::cin >> password;

    if (
        username.empty() ||
        password.empty()
    )
    {
        std::cerr
            << "Username and password "
            << "cannot be empty."
            << std::endl;

        return 1;
    }

    // Log in through the API Gateway to get a session token.
    // The game server only accepts this token, not the password.
    AuthResult authResult;

    try
    {
        const AuthClient authClient(
            AUTH_HOST,
            AUTH_PORT
        );

        authResult =
            authClient.loginOrRegister(
                username,
                password
            );
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "[UI] Could not reach the API Gateway: "
            << exception.what()
            << std::endl;

        return 1;
    }

    password.clear();

    if (!authResult.success)
    {
        std::cerr
            << "[UI] Login failed: "
            << authResult.errorMessage
            << std::endl;

        return 1;
    }

    std::cout
        << "[UI] Authenticated via API Gateway."
        << std::endl;

    // The token is sent in the password field,
    // which is what the game server expects.
    UiApplication application(
        SERVER_URI,
        ASSETS_DIRECTORY,
        username,
        authResult.token
    );

    return application.run();
}
