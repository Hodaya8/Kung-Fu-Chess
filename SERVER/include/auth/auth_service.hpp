#pragma once

#include <string>

#include "database/userRepository.hpp"
#include "tables/user_class.hpp"

// Represents all possible results of a login/registration attempt
enum class AuthStatus
{
    Authenticated,
    Registered,
    InvalidPassword,
    InvalidInput,
    DatabaseError
};

// Bundle returning both the action status and the user data (if successful)
struct AuthResult
{
    AuthStatus status = AuthStatus::DatabaseError;
    User user;

    // Checks if the user was successfully logged in or created
    constexpr bool isSuccess() const
    {
        return status == AuthStatus::Authenticated || status == AuthStatus::Registered;
    }
};

class AuthService
{
private:
    // Reference to the database repository (Dependency Injection)
    UserRepository& userRepository;

public:
    // Explicit constructor requiring an active user repository
    explicit AuthService(UserRepository& userRepository);

    // Attempts to log in; if user does not exist, registers a new account automatically
    AuthResult loginOrRegister(const std::string& username, const std::string& password);
};