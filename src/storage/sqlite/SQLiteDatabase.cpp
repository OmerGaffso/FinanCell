#include "storage/sqlite/SQLiteDatabase.h"

#include <sqlite3.h>

#include "application/PersistenceError.h"

SQLiteDatabase::SQLiteDatabase(const std::string& filePath)
{
    const int result = sqlite3_open_v2(
        filePath.c_str(),
        &m_database,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        nullptr);

    if (result != SQLITE_OK)
    {
        const std::string errorMessage =
            "Failed to open database: " +
            std::string(m_database ? sqlite3_errmsg(m_database) : "unknown error");

        if (m_database)
        {
            sqlite3_close(m_database);
            m_database = nullptr;
        }

        throw PersistenceError(errorMessage);
    }

    if (sqlite3_busy_timeout(m_database, 3000) != SQLITE_OK)
    {
        const std::string message =
            "Failed to configure database timeout: " + std::string(sqlite3_errmsg(m_database));
        sqlite3_close(m_database);
        m_database = nullptr;
        throw PersistenceError(message);
    }
    execute("PRAGMA foreign_keys = ON;");
}

SQLiteDatabase::~SQLiteDatabase()
{
    if (m_database)
    {
        sqlite3_close_v2(m_database);
    }
}

void SQLiteDatabase::execute(const std::string& sql)
{
    char* sqliteError = nullptr;
    const int result =
        sqlite3_exec(m_database, sql.c_str(), nullptr, nullptr, &sqliteError);

    if (result != SQLITE_OK)
    {
        std::string errorMessage = "SQLite error: ";
        if (sqliteError)
        {
            errorMessage += sqliteError;
            sqlite3_free(sqliteError);
        }

        throw PersistenceError(errorMessage);
    }
}

std::uint64_t SQLiteDatabase::lastInsertId() const
{
    const sqlite3_int64 id = sqlite3_last_insert_rowid(m_database);
    if (id < 0)
    {
        throw PersistenceError("SQLite returned a negative row ID.");
    }

    return static_cast<std::uint64_t>(id);
}

std::uint64_t SQLiteDatabase::changedRowCount() const
{
    return static_cast<std::uint64_t>(sqlite3_changes(m_database));
}

sqlite3* SQLiteDatabase::handle() const
{
    return m_database;
}
