#include "auth/auth_service.hpp"

#include <optional>
#include <string>

AuthService::AuthService(UserRepository& userRepository)
    : userRepository(userRepository)
{
}

AuthResult AuthService::loginOrRegister(const std::string& username, const std::string& password)
{
    //Validate that the input strings are not empty
    if (username.empty() || password.empty())
    {
        return AuthResult{AuthStatus::InvalidInput, User{}};
    }

    //Search SQLite database for an existing user with this username
    const std::optional<User> existingUser = userRepository.findByUsername(username);

    // User exists -> Verify password
    if (existingUser.has_value())
    {
        // Compare password against the stored hash
        if (existingUser->passwordHash != password)
        {
            return AuthResult{AuthStatus::InvalidPassword, User{}};
        }

        // Passwords match -> Return authenticated status and existing user data
        return AuthResult{AuthStatus::Authenticated, existingUser.value()};
    }

    // User does not exist -> Create a new user account with initial ELO rating (1200)
    User newUser(0, username, password, User::INITIAL_RATING);

    // Attempt to save the new user into the database
    if (!userRepository.addUser(newUser))
    {
        return AuthResult{AuthStatus::DatabaseError, User{}};
    }

    //Fetch the newly inserted user from DB to obtain their auto-generated SQLite ID
    const std::optional<User> createdUser = userRepository.findByUsername(username);

    if (!createdUser.has_value())
    {
        return AuthResult{AuthStatus::DatabaseError, User{}};
    }

    // Registration successful -> Return registered status and full user profile
    return AuthResult{AuthStatus::Registered, createdUser.value()};
}