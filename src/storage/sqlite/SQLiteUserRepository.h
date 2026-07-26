#pragma once

#include <string>

#include "application/UserRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"

class SQLiteUserRepository final : public UserRepository
{
public:
    explicit SQLiteUserRepository(SQLiteDatabase& database);
    bool insertUser(
        const std::string& username,
        const std::string& displayName,
        const std::string& passwordHash) override;
    bool userExists(const std::string& username) const override;
    std::optional<User> findUserById(std::uint64_t userId) const override;
    std::optional<User> findUserByUsername(const std::string& username) const override;
    std::vector<User> findAllUsers() const override;

private:
    SQLiteDatabase& m_database;
};
