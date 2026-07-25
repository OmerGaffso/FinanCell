#include <stdexcept>
#include <iostream>
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

bool SQLiteStorage::insertUser(const std::string& username, const std::string& displayName, const std::string& passwordHash)
{
    const std::string sql = "INSERT INTO users (username, display_name, password_hash) VALUES ('" +
                            username + "', '" + displayName + "', '" + passwordHash + "');";

    try
    {
        executeSQL(sql);
        return true;
    }
    catch (const std::runtime_error& e)
    {
        // Handle the error (e.g., log it) if needed
        return false;
    }
}

bool SQLiteStorage::isUserExists(const std::string& username)
{
    const std::string sql = "SELECT COUNT(*) FROM users WHERE username = '" + username + "';";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare SQL statement: " + std::string(sqlite3_errmsg(m_db)));
    }

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int count = sqlite3_column_int(stmt, 0);
        exists = (count > 0);
    }

    sqlite3_finalize(stmt);
    return exists;
}

User* SQLiteStorage::findUserByUsername(const std::string& username)
{
    const std::string sql = "SELECT id, username, display_name, password_hash FROM users WHERE username = '" + username + "';";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare SQL statement: " + std::string(sqlite3_errmsg(m_db)));
    }

    User* user = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char* dbUsername = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* dbDisplayName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* dbPasswordHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        user = new User(id, dbUsername ? dbUsername : "", dbDisplayName ? dbDisplayName : "", dbPasswordHash ? dbPasswordHash : "");
    }

    sqlite3_finalize(stmt);
    return user;
}

void SQLiteStorage::printUsers() const
{
    const std::string sql = "SELECT id, username, display_name FROM users;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare SQL statement: " + std::string(sqlite3_errmsg(m_db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char* dbUsername = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* dbDisplayName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        std::cout << "ID: " << id 
                  << ", Username: " << (dbUsername ? dbUsername : "") 
                  << ", Display Name: " << (dbDisplayName ? dbDisplayName : "") 
                  << std::endl;
    }

    sqlite3_finalize(stmt);
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