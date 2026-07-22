#include "auth/auth_service.hpp"

#include <optional>
#include <string>

AuthService::AuthService(
    UserRepository& userRepository)
    : userRepository(userRepository)
{
}

AuthResult AuthService::loginOrRegister(
    const std::string& username,
    const std::string& password)
{
    if (
        username.empty() ||
        password.empty()
    )
    {
        return AuthResult{
            AuthStatus::InvalidInput,
            User{}
        };
    }

    const std::optional<User> existingUser =
        userRepository.findByUsername(
            username
        );

    if (existingUser.has_value())
    {
        if (
            existingUser->password !=
            password
        )
        {
            return AuthResult{
                AuthStatus::InvalidPassword,
                User{}
            };
        }

        return AuthResult{
            AuthStatus::Authenticated,
            existingUser.value()
        };
    }

    User newUser;

    newUser.username = username;
    newUser.password = password;
    newUser.rating =
        User::INITIAL_RATING;

    if (!userRepository.addUser(
            newUser))
    {
        return AuthResult{
            AuthStatus::DatabaseError,
            User{}
        };
    }

    const std::optional<User> createdUser =
        userRepository.findByUsername(
            username
        );

    if (!createdUser.has_value())
    {
        return AuthResult{
            AuthStatus::DatabaseError,
            User{}
        };
    }

    return AuthResult{
        AuthStatus::Registered,
        createdUser.value()
    };
}