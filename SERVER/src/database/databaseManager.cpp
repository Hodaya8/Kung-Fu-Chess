#include "database/databaseManager.hpp"

#include <iostream>
#include <utility>

DatabaseManager::DatabaseManager(std::string databasePath)
    : database(nullptr),
      databasePath(std::move(databasePath))
{
}

DatabaseManager::~DatabaseManager()
{
    // Ensure the database is safely closed when the manager is destroyed
    if (database)
    {
        sqlite3_close(database);
        database = nullptr;
        std::cout << "[DATABASE] Connection closed." << std::endl;
    }
}

bool DatabaseManager::init()
{
    // Avoid re-opening if the connection is already active
    if (database)
    {
        return true;
    }

    // Attempt to open the SQLite database file
    const int result = sqlite3_open(databasePath.c_str(), &database);

    if (result != SQLITE_OK)
    {
        std::cerr << "[DATABASE] Could not open database: "
                  << (database ? sqlite3_errmsg(database) : "Unknown error")
                  << std::endl;

        if (database)
        {
            sqlite3_close(database);
            database = nullptr;
        }

        return false;
    }

    std::cout << "[DATABASE] Opened: " << databasePath << std::endl;
    return true;
}

sqlite3* DatabaseManager::getConnection() const
{
    return database;
}

bool DatabaseManager::isOpen() const
{
    return database != nullptr;
}