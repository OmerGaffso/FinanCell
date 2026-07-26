#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/User.h"

class UserRepository
{
public:
    virtual ~UserRepository() = default;

    virtual bool insertUser(
        const std::string& username,
        const std::string& displayName,
        const std::string& passwordHash) = 0;
    virtual bool userExists(const std::string& username) const = 0;
    virtual std::optional<User> findUserById(std::uint64_t userId) const = 0;
    virtual std::optional<User> findUserByUsername(const std::string& username) const = 0;
    virtual std::vector<User> findAllUsers() const = 0;
};
