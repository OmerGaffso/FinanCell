#include "storage/sqlite/SQLiteUserRepository.h"

#include <cstdint>
#include <stdexcept>
#include <string>

#include "storage/sqlite/Schema.h"

namespace
{
class Statement
{
public:
    Statement(sqlite3* database, const char* sql)
    {
        if (sqlite3_prepare_v2(database, sql, -1, &m_statement, nullptr) != SQLITE_OK)
        {
            throw std::runtime_error(
                "Failed to prepare SQL statement: " +
                std::string(sqlite3_errmsg(database)));
        }
    }

    ~Statement()
    {
        sqlite3_finalize(m_statement);
    }

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    sqlite3_stmt* get() const
    {
        return m_statement;
    }

private:
    sqlite3_stmt* m_statement{nullptr};
};

void bindText(
    sqlite3* database,
    sqlite3_stmt* statement,
    int index,
    const std::string& value)
{
    if (sqlite3_bind_text(
            statement,
            index,
            value.c_str(),
            static_cast<int>(value.size()),
            SQLITE_TRANSIENT) != SQLITE_OK)
    {
        throw std::runtime_error(
            "Failed to bind SQL value: " +
            std::string(sqlite3_errmsg(database)));
    }
}

std::string readText(sqlite3_stmt* statement, int column)
{
    const unsigned char* text = sqlite3_column_text(statement, column);
    return text ? reinterpret_cast<const char*>(text) : "";
}
}

SQLiteUserRepository::SQLiteUserRepository(const std::string& dbFilePath)
    : m_dbFilePath(dbFilePath)
{
    const int result = sqlite3_open_v2(
        m_dbFilePath.c_str(),
        &m_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        nullptr);

    if (result != SQLITE_OK)
    {
        const std::string errorMessage =
            "Failed to open database: " + std::string(sqlite3_errmsg(m_db));

        if (m_db)
        {
            sqlite3_close(m_db);
            m_db = nullptr;
        }

        throw std::runtime_error(errorMessage);
    }
}

SQLiteUserRepository::~SQLiteUserRepository()
{
    if (m_db)
    {
        sqlite3_close(m_db);
    }
}

void SQLiteUserRepository::initializeDatabase()
{
    executeSQL(Schema::CREATE_USERS_TABLE);
}

bool SQLiteUserRepository::insertUser(
    const std::string& username,
    const std::string& displayName,
    const std::string& passwordHash)
{
    constexpr char sql[] =
        "INSERT INTO users (username, display_name, password_hash) "
        "VALUES (?, ?, ?);";

    Statement statement(m_db, sql);
    bindText(m_db, statement.get(), 1, username);
    bindText(m_db, statement.get(), 2, displayName);
    bindText(m_db, statement.get(), 3, passwordHash);

    return sqlite3_step(statement.get()) == SQLITE_DONE;
}

bool SQLiteUserRepository::userExists(const std::string& username) const
{
    constexpr char sql[] =
        "SELECT 1 FROM users WHERE username = ? LIMIT 1;";

    Statement statement(m_db, sql);
    bindText(m_db, statement.get(), 1, username);

    return sqlite3_step(statement.get()) == SQLITE_ROW;
}

std::optional<User> SQLiteUserRepository::findUserByUsername(
    const std::string& username) const
{
    constexpr char sql[] =
        "SELECT id, username, display_name, password_hash "
        "FROM users WHERE username = ?;";

    Statement statement(m_db, sql);
    bindText(m_db, statement.get(), 1, username);

    if (sqlite3_step(statement.get()) != SQLITE_ROW)
    {
        return std::nullopt;
    }

    const std::uint64_t id =
        static_cast<std::uint64_t>(sqlite3_column_int64(statement.get(), 0));

    return User(
        id,
        readText(statement.get(), 1),
        readText(statement.get(), 2),
        readText(statement.get(), 3));
}

std::vector<User> SQLiteUserRepository::findAllUsers() const
{
    constexpr char sql[] =
        "SELECT id, username, display_name, password_hash "
        "FROM users ORDER BY id;";

    Statement statement(m_db, sql);
    std::vector<User> users;

    while (sqlite3_step(statement.get()) == SQLITE_ROW)
    {
        const std::uint64_t id =
            static_cast<std::uint64_t>(sqlite3_column_int64(statement.get(), 0));

        users.emplace_back(
            id,
            readText(statement.get(), 1),
            readText(statement.get(), 2),
            readText(statement.get(), 3));
    }

    return users;
}

void SQLiteUserRepository::executeSQL(const std::string& sql)
{
    char* errorMessage = nullptr;
    const int result =
        sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK)
    {
        std::string message = "SQL error: ";
        if (errorMessage)
        {
            message += errorMessage;
            sqlite3_free(errorMessage);
        }

        throw std::runtime_error(message);
    }
}
