#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/User.h"

/** @brief Persistence contract for user accounts. */
class UserRepository
{
public:
    /** @brief Destroys the repository interface. */
    virtual ~UserRepository() = default;

    /** @brief Inserts a user. @param username Username. @param displayName Display name. @param passwordHash Encoded hash. @return True on success. */
    virtual bool insertUser(
        const std::string& username,
        const std::string& displayName,
        const std::string& passwordHash) = 0;
    /** @brief Checks username existence. @param username Username. @return True when registered. */
    virtual bool userExists(const std::string& username) const = 0;
    /** @brief Finds a user by ID. @param userId User ID. @return Matching user, or empty. */
    virtual std::optional<User> findUserById(std::uint64_t userId) const = 0;
    /** @brief Finds a user by username. @param username Username. @return Matching user, or empty. */
    virtual std::optional<User> findUserByUsername(const std::string& username) const = 0;
    /** @brief Replaces a password hash. @param userId User ID. @param passwordHash Encoded hash. @return True when updated. */
    virtual bool updatePasswordHash(std::uint64_t userId, const std::string& passwordHash) = 0;
    /** @brief Returns every user. @return All stored users. */
    virtual std::vector<User> findAllUsers() const = 0;
};
