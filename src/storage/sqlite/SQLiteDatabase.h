#pragma once

#include <cstdint>
#include <string>

struct sqlite3;

/** RAII owner of a SQLite connection shared by repositories. */
class SQLiteDatabase
{
public:
    explicit SQLiteDatabase(const std::string& filePath);
    ~SQLiteDatabase();

    SQLiteDatabase(const SQLiteDatabase&) = delete;
    SQLiteDatabase& operator=(const SQLiteDatabase&) = delete;

    void execute(const std::string& sql);
    std::uint64_t lastInsertId() const;
    std::uint64_t changedRowCount() const;
    sqlite3* handle() const;

private:
    sqlite3* m_database{nullptr};
};
