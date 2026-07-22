#include <string>

#include "application/server_application.hpp"

namespace
{
    constexpr int SERVER_PORT = 9002;

    const std::string DATABASE_PATH =
        "server_data.db";
}

int main()
{
    KungFuChessServerApplication application(
        SERVER_PORT,
        DATABASE_PATH
    );

    return application.run();
}