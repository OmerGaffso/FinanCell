#include "UserService.h"
#include <iostream>
#include <string>

UserService::UserService() {};

bool UserService::createUser(std::string& username, std::string& displayName, std::string& password)
{
    username = normalizeText(username);
    displayName = trim(displayName);
    password = trim(password);
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

bool UserService::userExists(const std::string& username) const
{
    for (const User& user : m_users)
    {
        if (user.getUsername() == username)
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
        std::cout << "Username: " << user.getUsername() << 
                    ", Display Name: " << user.getDisplayName() << std::endl;
    }
}

std::string UserService::normalizeText(const std::string& text) const
{
    return toLower(trim(text));
}

std::string UserService::trim(const std::string& str) const
{
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start != std::string::npos && end != std::string::npos)
    {
        return str.substr(start, end - start + 1);
    }
    return "";
}

std::string UserService::toLower(const std::string& str) const
{
    std::string lowerStr = str;
    for (char& c : lowerStr)
    {
        c = std::tolower(c);
    }
    return lowerStr;
}
