#pragma once

#include <string>
#include <utility>

struct User
{
    // Default starting rating for all new players
    static constexpr int INITIAL_RATING = 1200;
    int id = 0;
    std::string username;
    std::string passwordHash;
    // Player's current chess rating
    int rating = INITIAL_RATING;
    // Default constructor
    User() = default;
    // Constructor that initializes all fields using move semantics
    User(int id, std::string username, std::string passwordHash, int rating = INITIAL_RATING)
        : id(id),
          username(std::move(username)),
          passwordHash(std::move(passwordHash)),
          rating(rating)
    {
    }

    // Equality operator to compare two User objects 
    bool operator==(const User& other) const {
    return id == other.id && 
           username == other.username && 
           passwordHash == other.passwordHash && 
           rating == other.rating;
}
};