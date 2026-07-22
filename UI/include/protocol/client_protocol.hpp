#pragma once

#include <string>

#include "input/mouse_input.hpp"

namespace ClientProtocol
{
    std::string createLoginRequestMessage(
        const std::string& username,
        const std::string& password
    );

    std::string createClickRequestMessage(
        const MouseClick& click
    );
}