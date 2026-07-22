#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include <string>
#include "sqlite3.h"

class DatabaseManager {
private:
    sqlite3* db;
    std::string dbName;

public:
    explicit DatabaseManager(std::string name);
    ~DatabaseManager();

    // פתיחת המסד ואתחול כל הטבלאות במערכת
    bool init();

    // החזרת מצביע החיבור הפעיל לשימוש ה-Repositories
    sqlite3* getConnection() const;
    static sqlite3* openDatabase(const std::string& dbPath);
    static void closeDatabase(sqlite3* db);
};

#endif // DATABASE_MANAGER_HPP