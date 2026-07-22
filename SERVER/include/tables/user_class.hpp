#pragma once

#include <string>

struct User
{
    static constexpr int INITIAL_RATING = 1200;

    int id = 0;
    std::string username;
    std::string password;
    int rating = INITIAL_RATING;
};