#pragma once

#include <string>
#include "sqlite3.h"
#include "../../domain/User.h"

struct sqlite3; // Forward declaration of sqlite3 struct

class SQLiteStorage
{
    public:
        explicit SQLiteStorage(const std::string& dbFilePath);
        ~SQLiteStorage();

        // Delete copy constructor and assignment operator to prevent copying
        SQLiteStorage(const SQLiteStorage&) = delete;
        SQLiteStorage& operator=(const SQLiteStorage&) = delete;

        void initializeDatabase();
        bool insertUser(const std::string& username, const std::string& displayName, const std::string& passwordHash);
        bool isUserExists(const std::string& username);
        User* findUserByUsername(const std::string& username);

        void printUsers() const;

    private:
        void executeSQL(const std::string& sql);

        std::string m_dbFilePath;
        sqlite3* m_db{nullptr};
};