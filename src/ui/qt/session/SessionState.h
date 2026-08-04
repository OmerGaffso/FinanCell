#pragma once

#include <cstdint>

#include <QString>

/** @brief Holds the authenticated identity shared by Qt-facing controllers. */
class SessionState final
{
public:
    /** @brief Returns whether a user is authenticated. @return True when a user is active. */
    bool loggedIn() const;
    /** @brief Returns the active user ID. @return User ID, or zero while logged out. */
    std::uint64_t userId() const;
    /** @brief Returns the active username. @return Username, or empty while logged out. */
    const QString& username() const;
    /** @brief Returns the active display name. @return Display name, or empty while logged out. */
    const QString& displayName() const;

    /** @brief Stores an authenticated identity. @param userId User ID. @param username Normalized username. @param displayName Display name. */
    void setUser(
        std::uint64_t userId,
        const QString& username,
        const QString& displayName);
    /** @brief Clears the authenticated identity. */
    void clear();

private:
    std::uint64_t m_userId{0};
    QString m_username;
    QString m_displayName;
};
