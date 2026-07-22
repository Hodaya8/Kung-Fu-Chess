#include "include/repositories/userRepository.hpp"
#include <iostream>

bool UserRepository::createTable(sqlite3* db) {
    const char* sql = "CREATE TABLE IF NOT EXISTS Users ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "Username TEXT UNIQUE NOT NULL, "
                      "Password TEXT NOT NULL, "
                      "Rating INTEGER DEFAULT 1200"
                      ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, 0, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error creating Users table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool UserRepository::addUser(sqlite3* db, const User& user) {
    const char* sql = "INSERT INTO Users (Username, Password, Rating) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.password.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, user.rating);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

bool UserRepository::getUser(sqlite3* db, const std::string& username, User& outUser) {
    const char* sql = "SELECT ID, Username, Password, Rating FROM Users WHERE Username = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outUser.id = sqlite3_column_int(stmt, 0);
        outUser.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        outUser.password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        outUser.rating = sqlite3_column_int(stmt, 3);
        found = true;
    }

    sqlite3_finalize(stmt);
    return found;
}