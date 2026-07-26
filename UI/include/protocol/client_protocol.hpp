#pragma once

#include <string>

#include "model/position.hpp"

namespace ClientProtocol
{
    std::string createLoginRequestMessage(
        const std::string& username,
        const std::string& password
    );

    std::string createMoveRequestMessage(
        const Position& source,
        const Position& destination
    );

    std::string createJumpRequestMessage(
        const Position& cell
    );
}