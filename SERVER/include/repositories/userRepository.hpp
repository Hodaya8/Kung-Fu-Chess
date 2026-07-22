#ifndef USER_REPOSITORY_HPP
#define USER_REPOSITORY_HPP

#include <string>
#include "../tables/user_class.hpp"
#include "sqlite3.h"

class UserRepository {
public:
    // יצירת טבלת המשתמשים אם אינה קיימת
    static bool createTable(sqlite3* db);

    // הוספת משתמש חדש
    static bool addUser(sqlite3* db, const User& user);

    // חיפוש משתמש לפי שמות משתמש
    static bool getUser(sqlite3* db, const std::string& username, User& outUser);
};

#endif // USER_REPOSITORY_HPP