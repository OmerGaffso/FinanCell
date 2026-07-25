#include "storage/sqlite/SQLiteStatement.h"

#include <limits>
#include <stdexcept>

SQLiteStatement::SQLiteStatement(
    SQLiteDatabase& database,
    const std::string& sql)
    : m_database(database)
{
    if (sqlite3_prepare_v2(
            m_database.handle(),
            sql.c_str(),
            -1,
            &m_statement,
            nullptr) != SQLITE_OK)
    {
        throwDatabaseError("Failed to prepare SQL statement");
    }
}

SQLiteStatement::~SQLiteStatement()
{
    sqlite3_finalize(m_statement);
}

void SQLiteStatement::bindText(int index, const std::string& value)
{
    if (sqlite3_bind_text(
            m_statement,
            index,
            value.c_str(),
            static_cast<int>(value.size()),
            SQLITE_TRANSIENT) != SQLITE_OK)
    {
        throwDatabaseError("Failed to bind text value");
    }
}

void SQLiteStatement::bindUInt64(int index, std::uint64_t value)
{
    constexpr std::uint64_t maxSQLiteInteger =
        static_cast<std::uint64_t>(std::numeric_limits<sqlite3_int64>::max());

    if (value > maxSQLiteInteger)
    {
        throw std::overflow_error("ID is too large for a SQLite INTEGER.");
    }

    if (sqlite3_bind_int64(
            m_statement,
            index,
            static_cast<sqlite3_int64>(value)) != SQLITE_OK)
    {
        throwDatabaseError("Failed to bind integer value");
    }
}

void SQLiteStatement::execute()
{
    if (sqlite3_step(m_statement) != SQLITE_DONE)
    {
        throwDatabaseError("Failed to execute SQL statement");
    }
}

bool SQLiteStatement::next()
{
    const int result = sqlite3_step(m_statement);
    if (result == SQLITE_ROW)
    {
        return true;
    }
    if (result == SQLITE_DONE)
    {
        return false;
    }

    throwDatabaseError("Failed to read SQL result");
    return false;
}

std::string SQLiteStatement::columnText(int column) const
{
    const unsigned char* text = sqlite3_column_text(m_statement, column);
    return text ? reinterpret_cast<const char*>(text) : "";
}

std::uint64_t SQLiteStatement::columnUInt64(int column) const
{
    const sqlite3_int64 value = sqlite3_column_int64(m_statement, column);
    if (value < 0)
    {
        throw std::runtime_error("SQLite returned a negative ID.");
    }

    return static_cast<std::uint64_t>(value);
}

void SQLiteStatement::throwDatabaseError(const std::string& context) const
{
    throw std::runtime_error(
        context + ": " + std::string(sqlite3_errmsg(m_database.handle())));
}
