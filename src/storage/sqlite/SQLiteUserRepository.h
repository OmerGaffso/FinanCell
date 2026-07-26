#pragma once

#include <string>

#include "application/UserRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"

/** @brief SQLite implementation of user persistence. */
class SQLiteUserRepository final : public UserRepository
{
public:
    /** @brief Creates the repository. @param database Shared database connection. */
    explicit SQLiteUserRepository(SQLiteDatabase& database);
    /** @copydoc UserRepository::insertUser */
    bool insertUser(
        const std::string& username,
        const std::string& displayName,
        const std::string& passwordHash) override;
    /** @copydoc UserRepository::userExists */
    bool userExists(const std::string& username) const override;
    /** @copydoc UserRepository::findUserById */
    std::optional<User> findUserById(std::uint64_t userId) const override;
    /** @copydoc UserRepository::findUserByUsername */
    std::optional<User> findUserByUsername(const std::string& username) const override;
private:
    SQLiteDatabase& m_database;
};
