#pragma once

#include <optional>
#include <string>

#include "database/databaseManager.hpp"
#include "tables/user_class.hpp"

class UserRepository
{
private:
    DatabaseManager& databaseManager;

public:
    explicit UserRepository(
        DatabaseManager& databaseManager
    );

    bool createTable();

    bool addUser(
        const User& user
    );

    std::optional<User> findByUsername(
        const std::string& username
    ) const;

    bool updateRating(
        int userId,
        int newRating
    );
};