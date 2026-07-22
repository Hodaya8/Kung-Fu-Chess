#include "include/database/databaseManager.hpp"
#include "include/repositories/userRepository.hpp"
#include "sqlite3.h"
#include <iostream>

DatabaseManager::DatabaseManager(std::string name) : db(nullptr), dbName(std::move(name)) {}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
        std::cout << "Database connection closed." << std::endl;
    }
}

bool DatabaseManager::init() {
    // פתיחה או יצירה של קובץ מסד הנתונים המקומי
    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << (db ? sqlite3_errmsg(db) : "Unknown error") << std::endl;
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
        return false;
    }

    std::cout << "Database opened successfully: " << dbName << std::endl;

    // הפעלת יצירת טבלת המשתמשים דרך ה-Repository שלה
    if (!UserRepository::createTable(db)) {
        std::cerr << "Failed to initialize Users table." << std::endl;
        return false;
    }

    std::cout << "All database tables initialized successfully." << std::endl;
    return true;
}

sqlite3* DatabaseManager::getConnection() const {
    return db;
}

sqlite3* DatabaseManager::openDatabase(const std::string& dbPath) {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

void DatabaseManager::closeDatabase(sqlite3* db) {
    if (db) {
        sqlite3_close(db);
    }
}