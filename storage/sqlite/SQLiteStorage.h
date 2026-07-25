#pragma once

#include <string>

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

    private:
        void executeSQL(const std::string& sql);

        std::string m_dbFilePath;
        sqlite3* m_db{nullptr};
};