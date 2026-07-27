#pragma once

#include <cstdint>
#include <string>
#include <utility>

/** @brief Public account information suitable for an authenticated user directory. */
class UserSummary
{
public:
    /** @brief Constructs a public user summary. @param userId User ID. @param username Username. @param displayName Display name. */
    UserSummary(std::uint64_t userId, std::string username, std::string displayName)
        : m_userId(userId),
          m_username(std::move(username)),
          m_displayName(std::move(displayName))
    {
    }

    /** @brief Returns the user ID. @return User ID. */
    std::uint64_t getUserId() const { return m_userId; }
    /** @brief Returns the username. @return Username. */
    const std::string& getUsername() const { return m_username; }
    /** @brief Returns the display name. @return Display name. */
    const std::string& getDisplayName() const { return m_displayName; }

private:
    std::uint64_t m_userId;
    std::string m_username;
    std::string m_displayName;
};
