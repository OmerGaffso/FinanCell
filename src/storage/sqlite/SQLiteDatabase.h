#pragma once

#include <cstdint>
#include <string>

struct sqlite3;

/** @brief RAII owner of a SQLite connection shared by repositories. */
class SQLiteDatabase
{
public:
    /** @brief Opens or creates a database. @param filePath Database path or :memory:. @throws std::runtime_error When opening fails. */
    explicit SQLiteDatabase(const std::string& filePath);
    /** @brief Closes the database connection. */
    ~SQLiteDatabase();

    /** @brief Copy construction is disabled. */
    SQLiteDatabase(const SQLiteDatabase&) = delete;
    /** @brief Copy assignment is disabled. @return This object. */
    SQLiteDatabase& operator=(const SQLiteDatabase&) = delete;

    /** @brief Executes SQL. @param sql SQL text. @throws std::runtime_error On SQLite errors. */
    void execute(const std::string& sql);
    /** @brief Returns the last generated row ID. @return Non-negative row ID. */
    std::uint64_t lastInsertId() const;
    /** @brief Returns affected rows. @return Rows changed by the last statement. */
    std::uint64_t changedRowCount() const;
    /** @brief Returns the native handle. @return SQLite connection handle. */
    sqlite3* handle() const;

private:
    sqlite3* m_database{nullptr};
};
