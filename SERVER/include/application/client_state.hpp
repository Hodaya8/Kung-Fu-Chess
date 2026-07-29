#pragma once

#include <optional>
#include <string>

#include "model/piece.hpp"
#include "tables/user_class.hpp"

struct ClientState
{
    int userId = 0;
    std::string username;
    int rating = User::INITIAL_RATING;

    std::optional<int> roomId;
    std::optional<Color> playerColor;

    bool loggedIn = false;

    bool isInRoom() const
    {
        return roomId.has_value();
    }

    bool isPlayer() const
    {
        return
            roomId.has_value() &&
            playerColor.has_value();
    }

    bool isSpectator() const
    {
        return
            roomId.has_value() &&
            !playerColor.has_value();
    }
};