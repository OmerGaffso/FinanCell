#include "domain/User.h"

User::User(const std::string& username, const std::string& displayName, const std::string& password)
    :m_username(username), m_displayName(displayName), m_passwordHash(password)
{
    static std::uint64_t nextUserId = 1; // Static variable to keep track of the next user ID
    m_userId = nextUserId++; // Assign the current value and then increment for the
}

User::User(std::uint64_t userId, const std::string& username, const std::string& displayName, const std::string& password)
    : m_username(username), m_displayName(displayName), m_userId(userId), m_passwordHash(password)
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

std::uint64_t User::getUserId() const
{
    return m_userId;
}

const std::string& User::getPasswordHash() const
{
    return m_passwordHash;
}
