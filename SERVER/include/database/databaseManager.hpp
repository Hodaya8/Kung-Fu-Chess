#pragma once

#include <string>

#include "sqlite3.h"

class DatabaseManager
{
private:
    // Raw pointer to the internal SQLite database connection
    sqlite3* database = nullptr;
    
    // File path to the SQLite database
    std::string databasePath;

public:
    //constructor
    explicit DatabaseManager(std::string databasePath);

    // Destructor ensures the SQLite connection is safely closed
    ~DatabaseManager();

    // Prevent copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Opens the database file and initializes the connection
    bool init();

    // Returns the raw SQLite pointer for repositories to execute queries
    sqlite3* getConnection() const;

    // Checks if the database connection is currently active and valid
    bool isOpen() const;
};