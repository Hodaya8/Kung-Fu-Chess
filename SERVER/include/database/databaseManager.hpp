#pragma once

#include <string>

#include "sqlite3.h"

class DatabaseManager
{
private:
    sqlite3* database;
    std::string databasePath;

public:
    explicit DatabaseManager(
        std::string databasePath
    );

    ~DatabaseManager();

    DatabaseManager(
        const DatabaseManager&
    ) = delete;

    DatabaseManager& operator=(
        const DatabaseManager&
    ) = delete;

    bool init();

    sqlite3* getConnection() const;

    bool isOpen() const;
};