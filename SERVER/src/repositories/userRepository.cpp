#include "repositories/userRepository.hpp"

#include <iostream>

#include "sqlite3.h"

UserRepository::UserRepository(
    DatabaseManager& databaseManager)
    : databaseManager(databaseManager)
{
}

bool UserRepository::createTable()
{
    sqlite3* database =
        databaseManager.getConnection();

    if (!database)
    {
        std::cerr
            << "[USER REPOSITORY] "
            << "Database is not open."
            << std::endl;

        return false;
    }

    const char* sql =
        "CREATE TABLE IF NOT EXISTS Users ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Username TEXT UNIQUE NOT NULL, "
        "Password TEXT NOT NULL, "
        "Rating INTEGER NOT NULL DEFAULT 1200"
        ");";

    char* errorMessage = nullptr;

    const int result =
        sqlite3_exec(
            database,
            sql,
            nullptr,
            nullptr,
            &errorMessage
        );

    if (result != SQLITE_OK)
    {
        std::cerr
            << "[USER REPOSITORY] "
            << "Could not create Users table: "
            << (
                errorMessage
                    ? errorMessage
                    : "Unknown error"
            )
            << std::endl;

        sqlite3_free(errorMessage);

        return false;
    }

    return true;
}

bool UserRepository::addUser(
    const User& user)
{
    sqlite3* database =
        databaseManager.getConnection();

    if (!database)
    {
        return false;
    }

    const char* sql =
        "INSERT INTO Users "
        "(Username, Password, Rating) "
        "VALUES (?, ?, ?);";

    sqlite3_stmt* statement = nullptr;

    if (
        sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr
        ) != SQLITE_OK
    )
    {
        std::cerr
            << "[USER REPOSITORY] "
            << "Could not prepare addUser: "
            << sqlite3_errmsg(database)
            << std::endl;

        return false;
    }

    sqlite3_bind_text(
        statement,
        1,
        user.username.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement,
        2,
        user.password.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        statement,
        3,
        user.rating
    );

    const bool success =
        sqlite3_step(statement) ==
            SQLITE_DONE;

    if (!success)
    {
        std::cerr
            << "[USER REPOSITORY] "
            << "Could not add user: "
            << sqlite3_errmsg(database)
            << std::endl;
    }

    sqlite3_finalize(statement);

    return success;
}

std::optional<User>
UserRepository::findByUsername(
    const std::string& username) const
{
    sqlite3* database =
        databaseManager.getConnection();

    if (!database)
    {
        return std::nullopt;
    }

    const char* sql =
        "SELECT ID, Username, Password, Rating "
        "FROM Users "
        "WHERE Username = ?;";

    sqlite3_stmt* statement = nullptr;

    if (
        sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr
        ) != SQLITE_OK
    )
    {
        std::cerr
            << "[USER REPOSITORY] "
            << "Could not prepare findByUsername: "
            << sqlite3_errmsg(database)
            << std::endl;

        return std::nullopt;
    }

    sqlite3_bind_text(
        statement,
        1,
        username.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    std::optional<User> result;

    if (
        sqlite3_step(statement) ==
        SQLITE_ROW
    )
    {
        User user;

        user.id =
            sqlite3_column_int(
                statement,
                0
            );

        const unsigned char* usernameText =
            sqlite3_column_text(
                statement,
                1
            );

        const unsigned char* passwordText =
            sqlite3_column_text(
                statement,
                2
            );

        user.username =
            usernameText
                ? reinterpret_cast<
                      const char*>(
                      usernameText
                  )
                : "";

        user.password =
            passwordText
                ? reinterpret_cast<
                      const char*>(
                      passwordText
                  )
                : "";

        user.rating =
            sqlite3_column_int(
                statement,
                3
            );

        result = user;
    }

    sqlite3_finalize(statement);

    return result;
}

bool UserRepository::updateRating(
    int userId,
    int newRating)
{
    sqlite3* database =
        databaseManager.getConnection();

    if (!database)
    {
        return false;
    }

    const char* sql =
        "UPDATE Users "
        "SET Rating = ? "
        "WHERE ID = ?;";

    sqlite3_stmt* statement = nullptr;

    if (
        sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr
        ) != SQLITE_OK
    )
    {
        std::cerr
            << "[USER REPOSITORY] "
            << "Could not prepare updateRating: "
            << sqlite3_errmsg(database)
            << std::endl;

        return false;
    }

    sqlite3_bind_int(
        statement,
        1,
        newRating
    );

    sqlite3_bind_int(
        statement,
        2,
        userId
    );

    const bool success =
        sqlite3_step(statement) ==
            SQLITE_DONE;

    if (!success)
    {
        std::cerr
            << "[USER REPOSITORY] "
            << "Could not update rating: "
            << sqlite3_errmsg(database)
            << std::endl;
    }

    sqlite3_finalize(statement);

    return success;
}