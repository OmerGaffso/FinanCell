#pragma once

#include <string>
#include <cstdint>

/** @brief User account data loaded from persistence. */
class User
{
    public:
        /** @brief Constructs a transient user. @param username Username. @param displayName Display name. @param password Password value. */
        User(const std::string& username, const std::string& displayName, const std::string& password);
        /** @brief Constructs a persisted user. @param userId User ID. @param username Username. @param displayName Display name. @param password Stored password hash. */
        User(std::uint64_t userId, const std::string& username, const std::string& displayName, const std::string& password);
        /** @brief Returns the username. @return Username. */
        std::string getUsername() const;
        /** @brief Returns the display name. @return Display name. */
        std::string getDisplayName() const;
        /** @brief Returns the user ID. @return User ID. */
        std::uint64_t getUserId() const;
        /** @brief Returns the password hash. @return Encoded password hash. */
        const std::string& getPasswordHash() const;

    private:
        std::string m_username;
        std::string m_displayName;
        std::uint64_t m_userId;
        std::string m_passwordHash;
};
