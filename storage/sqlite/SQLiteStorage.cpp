#include <stdexcept>
#include "sqlite3.h"

#include "SQLiteStorage.h"
#include "../../db/schema.h"

SQLiteStorage::SQLiteStorage(const std::string& dbFilePath)
    : m_dbFilePath(dbFilePath), m_db(nullptr)
{
    // Open the SQLite database
    const int res = sqlite3_open_v2(
                            m_dbFilePath.c_str(), &m_db, 
                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

    if (res != SQLITE_OK)
    {
        const std::string errorMsg = "Failed to open database: " + std::string(sqlite3_errmsg(m_db));

        if (m_db)
        {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
        throw std::runtime_error(errorMsg);
    }
}

SQLiteStorage::~SQLiteStorage()
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

void SQLiteStorage::initializeDatabase()
{
    executeSQL(Schema::CREATE_USERS_TABLE);
}

void SQLiteStorage::executeSQL(const std::string& sql)
{
    char* errMsg = nullptr;
    const int res = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (res != SQLITE_OK)
    {
        std::string errorMsg = "SQL error: ";
        if (errMsg)
        {
            errorMsg += errMsg;
            sqlite3_free(errMsg);
        }
        throw std::runtime_error(errorMsg);
    }
}