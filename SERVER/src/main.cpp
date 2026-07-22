#include "application/server_application.hpp"

namespace
{
    constexpr int SERVER_PORT = 9002;
}

int main()
{
    KungFuChessServerApplication application(
        SERVER_PORT
    );

    return application.run();
}