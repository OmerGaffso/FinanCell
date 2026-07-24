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
    for (const User& user : m_users)
    {
        if (user.getUsername() == username && user.checkPassword(password))
        {
            return true;
        }
    }

    return false;
}

void UserService::printUsers() const
{
    if (m_users.empty())
    {
        std::cout << "No users have been created.\n";
        return;
    }

    for (const User& user : m_users)
    {
        std::cout << user.getUsername() << '\n';
    }
}
