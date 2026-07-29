#include "database/userRepository.hpp"

#include <iostream>
#include <memory>
#include "sqlite3.h"

namespace
{
    struct StatementDeleter
    {
        void operator()(sqlite3_stmt* stmt) const
        {
            if (stmt)
            {
                sqlite3_finalize(stmt);
            }
        }
    };

    using ScopedStatement = std::unique_ptr<sqlite3_stmt, StatementDeleter>;
}

UserRepository::UserRepository(DatabaseManager& databaseManager)
    : databaseManager(databaseManager)
{
}

bool UserRepository::createTable()
{
    sqlite3* database = databaseManager.getConnection();
    if (!database)
    {
        std::cerr << "[USER REPOSITORY] Database is not open." << std::endl;
        return false;
    }

    // SQL query to initialize the Users table
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Users ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Username TEXT UNIQUE NOT NULL, "
        "PasswordHash TEXT NOT NULL, "
        "Rating INTEGER NOT NULL DEFAULT 1200"
        ");";

    char* errorMessage = nullptr;
    const int result = sqlite3_exec(database, sql, nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK)
    {
        std::cerr << "[USER REPOSITORY] Could not create Users table: "
                  << (errorMessage ? errorMessage : "Unknown error")
                  << std::endl;

        sqlite3_free(errorMessage);
        return false;
    }

    return true;
}

bool UserRepository::addUser(const User& user)
{
    sqlite3* database = databaseManager.getConnection();
    if (!database)
    {
        return false;
    }

    const char* sql = "INSERT INTO Users (Username, PasswordHash, Rating) VALUES (?, ?, ?);";
    sqlite3_stmt* rawStatement = nullptr;

    // Compile the SQL query into byte-code
    if (sqlite3_prepare_v2(database, sql, -1, &rawStatement, nullptr) != SQLITE_OK)
    {
        std::cerr << "[USER REPOSITORY] Could not prepare addUser: "
                  << sqlite3_errmsg(database) << std::endl;
        return false;
    }

    // Wrap the statement in  pointer so it auto-destroys when return
    ScopedStatement statement(rawStatement);

    // Bind parameters securely to prevent SQL Injection attacks
    // SQLITE_TRANSIENT tells SQLite to make its own private copy of the string
    sqlite3_bind_text(statement.get(), 1, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement.get(), 2, user.passwordHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement.get(), 3, user.rating);

    // Execute the insertion
    const bool success = (sqlite3_step(statement.get()) == SQLITE_DONE);
    if (!success)
    {
        std::cerr << "[USER REPOSITORY] Could not add user: "
                  << sqlite3_errmsg(database) << std::endl;
    }

    return success;
}

std::optional<User> UserRepository::findByUsername(const std::string& username) const
{
    sqlite3* database = databaseManager.getConnection();
    if (!database)
    {
        return std::nullopt;
    }

    const char* sql = "SELECT ID, Username, PasswordHash, Rating FROM Users WHERE Username = ?;";
    sqlite3_stmt* rawStatement = nullptr;

    if (sqlite3_prepare_v2(database, sql, -1, &rawStatement, nullptr) != SQLITE_OK)
    {
        std::cerr << "[USER REPOSITORY] Could not prepare findByUsername: "
                  << sqlite3_errmsg(database) << std::endl;
        return std::nullopt;
    }

    ScopedStatement statement(rawStatement);

    // Bind the searched username to the placeholder (?)
    sqlite3_bind_text(statement.get(), 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<User> result = std::nullopt;

    // Execute query and check if a matching row was found
    if (sqlite3_step(statement.get()) == SQLITE_ROW)
    {
        User user;
        user.id = sqlite3_column_int(statement.get(), 0);

        const unsigned char* usernameText = sqlite3_column_text(statement.get(), 1);
        const unsigned char* passwordText = sqlite3_column_text(statement.get(), 2);

        user.username = usernameText ? reinterpret_cast<const char*>(usernameText) : "";
        user.passwordHash = passwordText ? reinterpret_cast<const char*>(passwordText) : "";
        user.rating = sqlite3_column_int(statement.get(), 3);

        result = user;
    }

    return result;
}

bool UserRepository::updateRating(int userId, int newRating)
{
    sqlite3* database = databaseManager.getConnection();
    if (!database)
    {
        return false;
    }

    const char* sql = "UPDATE Users SET Rating = ? WHERE ID = ?;";
    sqlite3_stmt* rawStatement = nullptr;

    if (sqlite3_prepare_v2(database, sql, -1, &rawStatement, nullptr) != SQLITE_OK)
    {
        std::cerr << "[USER REPOSITORY] Could not prepare updateRating: "
                  << sqlite3_errmsg(database) << std::endl;
        return false;
    }

    ScopedStatement statement(rawStatement);

    // Bind parameters: 1 -> newRating, 2 -> userId
    sqlite3_bind_int(statement.get(), 1, newRating);
    sqlite3_bind_int(statement.get(), 2, userId);

    const bool success = (sqlite3_step(statement.get()) == SQLITE_DONE);
    if (!success)
    {
        std::cerr << "[USER REPOSITORY] Could not update rating: "
                  << sqlite3_errmsg(database) << std::endl;
    }

    return success;
}