#include "storage/sqlite/SQLiteUserRepository.h"

#include "storage/sqlite/SQLiteStatement.h"

SQLiteUserRepository::SQLiteUserRepository(SQLiteDatabase& database)
    : m_database(database)
{
}

bool SQLiteUserRepository::insertUser(
    const std::string& username,
    const std::string& displayName,
    const std::string& passwordHash)
{
    constexpr char sql[] =
        "INSERT INTO users (username, display_name, password_hash) "
        "VALUES (?, ?, ?);";

    SQLiteStatement statement(m_database, sql);
    statement.bindText(1, username);
    statement.bindText(2, displayName);
    statement.bindText(3, passwordHash);
    statement.execute();
    return true;
}

bool SQLiteUserRepository::userExists(const std::string& username) const
{
    constexpr char sql[] =
        "SELECT 1 FROM users WHERE username = ? LIMIT 1;";

    SQLiteStatement statement(m_database, sql);
    statement.bindText(1, username);
    return statement.next();
}

std::optional<User> SQLiteUserRepository::findUserByUsername(
    const std::string& username) const
{
    constexpr char sql[] =
        "SELECT id, username, display_name, password_hash "
        "FROM users WHERE username = ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindText(1, username);

    if (!statement.next())
    {
        return std::nullopt;
    }

    return User(
        statement.columnUInt64(0),
        statement.columnText(1),
        statement.columnText(2),
        statement.columnText(3));
}

std::vector<User> SQLiteUserRepository::findAllUsers() const
{
    constexpr char sql[] =
        "SELECT id, username, display_name, password_hash "
        "FROM users ORDER BY id;";

    SQLiteStatement statement(m_database, sql);
    std::vector<User> users;

    while (statement.next())
    {
        users.emplace_back(
            statement.columnUInt64(0),
            statement.columnText(1),
            statement.columnText(2),
            statement.columnText(3));
    }

    return users;
}
