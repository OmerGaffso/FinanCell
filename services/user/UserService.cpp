#include "UserService.h"
#include <iostream>

UserService::UserService() {};

bool UserService::createUser(const std::string& username, const std::string& displayName, const std::string& password)
{
    m_users.emplace_back(username, displayName, password);
    return true;
}

bool UserService::authenticateUser(const std::string& username, const std::string& password)
{
    // TODO - Check if the user exists and if the password matches.
    // If authentication is successful, return true; otherwise, return false.
    return true;
}

void UserService::printUsers() const
{
    for (const User& user : m_users)
    {
        std::cout << user.getUsername() << '\n';
    }
}
