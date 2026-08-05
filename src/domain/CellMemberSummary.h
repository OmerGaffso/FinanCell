#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "domain/CellRole.h"

/** @brief Public identity and role information for one financial-cell member. */
class CellMemberSummary
{
public:
    /** @brief Constructs a member summary. @param userId User ID. @param username Username. @param displayName Display name. @param role Cell role. */
    CellMemberSummary(
        std::uint64_t userId,
        std::string username,
        std::string displayName,
        CellRole role)
        : m_userId(userId),
          m_username(std::move(username)),
          m_displayName(std::move(displayName)),
          m_role(role)
    {
    }

    /** @brief Returns the user ID. @return User ID. */
    std::uint64_t getUserId() const { return m_userId; }
    /** @brief Returns the username. @return Username. */
    const std::string& getUsername() const { return m_username; }
    /** @brief Returns the display name. @return Display name. */
    const std::string& getDisplayName() const { return m_displayName; }
    /** @brief Returns the cell role. @return Role. */
    CellRole getRole() const { return m_role; }

private:
    std::uint64_t m_userId;
    std::string m_username;
    std::string m_displayName;
    CellRole m_role;
};
