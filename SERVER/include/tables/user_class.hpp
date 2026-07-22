#ifndef USER_H
#define USER_H

#include <string>

struct User {
    int id;
    std::string username;
    std::string password;
    int rating;
};

#endif // USER_H