#pragma once

#include <cstdint>
#include <string>

#include <sqlite3.h>

#include "storage/sqlite/SQLiteDatabase.h"

/** RAII wrapper for a prepared SQLite statement. */
class SQLiteStatement
{
public:
    SQLiteStatement(SQLiteDatabase& database, const std::string& sql);
    ~SQLiteStatement();

    SQLiteStatement(const SQLiteStatement&) = delete;
    SQLiteStatement& operator=(const SQLiteStatement&) = delete;

    void bindText(int index, const std::string& value);
    void bindUInt64(int index, std::uint64_t value);
    void bindInt64(int index, std::int64_t value);
    void execute();
    bool next();

    std::string columnText(int column) const;
    std::uint64_t columnUInt64(int column) const;
    std::int64_t columnInt64(int column) const;

private:
    void throwDatabaseError(const std::string& context) const;

    SQLiteDatabase& m_database;
    sqlite3_stmt* m_statement{nullptr};
};
