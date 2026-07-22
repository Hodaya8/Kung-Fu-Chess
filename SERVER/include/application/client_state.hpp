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

    std::optional<Color> playerColor;

    bool loggedIn = false;
};