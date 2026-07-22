#pragma once

#include <string>

#include "repositories/userRepository.hpp"
#include "tables/user_class.hpp"

enum class AuthStatus
{
    Authenticated,
    Registered,
    InvalidPassword,
    InvalidInput,
    DatabaseError
};

struct AuthResult
{
    AuthStatus status =
        AuthStatus::DatabaseError;

    User user;

    bool isSuccess() const
    {
        return
            status == AuthStatus::Authenticated ||
            status == AuthStatus::Registered;
    }
};

class AuthService
{
private:
    UserRepository& userRepository;

public:
    explicit AuthService(
        UserRepository& userRepository
    );

    AuthResult loginOrRegister(
        const std::string& username,
        const std::string& password
    );
};