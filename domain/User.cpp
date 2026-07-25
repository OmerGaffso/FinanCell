#include "User.h"

User::User(const std::string& username, const std::string& displayName, const std::string& password)
    :m_username(username), m_displayName(displayName), m_password(password)
{
    static std::uint64_t nextUserID = 1; // Static variable to keep track of the next user ID
    m_userID = nextUserID++; // Assign the current value and then increment for the
}

User::User(std::uint64_t userId, const std::string& username, const std::string& displayName, const std::string& password)
    : m_username(username), m_displayName(displayName), m_userID(userId), m_password(password)
{
}

std::string User::getUsername() const
{
    return m_username;
}

std::string User::getDisplayName() const
{
    return m_displayName;
}

std::uint64_t User::getUserID() const
{
    return m_userID;
}

bool User::checkPassword(const std::string& password) const
{
    return m_password == password;
}
