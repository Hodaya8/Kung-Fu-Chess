#pragma once

#include <optional>
#include <string>

#include "database/databaseManager.hpp"
#include "tables/user_class.hpp"

class UserRepository
{
private:
    // Reference to the shared database manager
    DatabaseManager& databaseManager;

public:
    // Constructor requiring an active DatabaseManager
    explicit UserRepository(DatabaseManager& databaseManager);

    // Creates the "users" table in SQLite if it does not already exist
    bool createTable();

    // Inserts a new user into the database; returns true if successful
    bool addUser(const User& user);

    // Searches for a user by username; returns std::nullopt if not found
    std::optional<User> findByUsername(const std::string& username) const;

    // Updates a player's rating after a match ends
    bool updateRating(int userId, int newRating);
};