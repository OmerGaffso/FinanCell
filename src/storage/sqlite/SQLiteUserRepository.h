#pragma once

#include <string>
#include <sqlite3.h>

#include "application/UserRepository.h"

class SQLiteUserRepository final : public UserRepository
{
public:
    explicit SQLiteUserRepository(const std::string& dbFilePath);
    ~SQLiteUserRepository() override;

    SQLiteUserRepository(const SQLiteUserRepository&) = delete;
    SQLiteUserRepository& operator=(const SQLiteUserRepository&) = delete;

    void initializeDatabase();
    bool insertUser(
        const std::string& username,
        const std::string& displayName,
        const std::string& passwordHash) override;
    bool userExists(const std::string& username) const override;
    std::optional<User> findUserByUsername(const std::string& username) const override;
    std::vector<User> findAllUsers() const override;

private:
    void executeSQL(const std::string& sql);

    std::string m_dbFilePath;
    sqlite3* m_db{nullptr};
};
