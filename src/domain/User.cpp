#include "domain/User.h"

User::User(
    std::uint64_t userId,
    const std::string& username,
    const std::string& displayName,
    const std::string& passwordHash)
    : m_username(username),
      m_displayName(displayName),
      m_userId(userId),
      m_passwordHash(passwordHash)
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
