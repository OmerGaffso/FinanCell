#pragma once

#include <cstdint>
#include <string>

#include <sqlite3.h>

#include "storage/sqlite/SQLiteDatabase.h"

/** @brief RAII wrapper for a prepared SQLite statement. */
class SQLiteStatement
{
public:
    /** @brief Prepares a statement. @param database Database connection. @param sql SQL text. */
    SQLiteStatement(SQLiteDatabase& database, const std::string& sql);
    /** @brief Finalizes the statement. */
    ~SQLiteStatement();

    /** @brief Copy construction is disabled. */
    SQLiteStatement(const SQLiteStatement&) = delete;
    /** @brief Copy assignment is disabled. @return This object. */
    SQLiteStatement& operator=(const SQLiteStatement&) = delete;

    /** @brief Binds text. @param index One-based index. @param value Text value. */
    void bindText(int index, const std::string& value);
    /** @brief Binds an unsigned ID. @param index One-based index. @param value ID value. */
    void bindUInt64(int index, std::uint64_t value);
    /** @brief Binds a signed integer. @param index One-based index. @param value Integer value. */
    void bindInt64(int index, std::int64_t value);
    /** @brief Executes a non-query statement. */
    void execute();
    /** @brief Advances a query. @return True when a row is available. */
    bool next();

    /** @brief Reads text. @param column Zero-based column. @return Text value. */
    std::string columnText(int column) const;
    /** @brief Reads an unsigned integer. @param column Zero-based column. @return Non-negative value. */
    std::uint64_t columnUInt64(int column) const;
    /** @brief Reads a signed integer. @param column Zero-based column. @return Signed value. */
    std::int64_t columnInt64(int column) const;

private:
    /** @brief Throws an SQLite error. @param context Operation context. @throws std::runtime_error Always. */
    void throwDatabaseError(const std::string& context) const;

    SQLiteDatabase& m_database;
    sqlite3_stmt* m_statement{nullptr};
};
