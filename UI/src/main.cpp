#include <iostream>
#include <string>

#include "application/ui_application.hpp"

namespace
{
    const std::string SERVER_URI =
        "ws://127.0.0.1:9002";

    const std::string ASSETS_DIRECTORY =
        R"(C:\Users\USER\Desktop\chess game\UI\asset)";
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

    UiApplication application(
        SERVER_URI,
        ASSETS_DIRECTORY,
        username,
        password
    );

    return application.run();
}