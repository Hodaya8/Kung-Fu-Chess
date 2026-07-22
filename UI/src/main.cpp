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
    UiApplication application(
        SERVER_URI,
        ASSETS_DIRECTORY
    );

    return application.run();
}