#pragma once

#include <string>

#include "input/mouse_input.hpp"

namespace ClientProtocol
{
    std::string createClickRequestMessage(
        const MouseClick& click
    );
}