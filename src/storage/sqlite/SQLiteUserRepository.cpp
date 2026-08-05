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

std::optional<User> SQLiteUserRepository::findUserById(
    std::uint64_t userId) const
{
    constexpr char sql[] =
        "SELECT id, username, display_name, password_hash "
        "FROM users WHERE id = ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, userId);
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

std::optional<UserSummary> SQLiteUserRepository::findUserSummaryById(
    std::uint64_t userId) const
{
    constexpr char sql[] =
        "SELECT id, username, display_name FROM users WHERE id = ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, userId);
    if (!statement.next()) return std::nullopt;

    return UserSummary(
        statement.columnUInt64(0),
        statement.columnText(1),
        statement.columnText(2));
}

std::vector<UserSummary> SQLiteUserRepository::findUserSummaries(
    const std::string& query,
    std::size_t limit) const
{
    constexpr char sql[] =
        "SELECT id, username, display_name FROM users "
        "WHERE ? = '' OR instr(lower(username), lower(?)) > 0 "
        "OR instr(lower(display_name), lower(?)) > 0 "
        "ORDER BY username COLLATE NOCASE, id LIMIT ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindText(1, query);
    statement.bindText(2, query);
    statement.bindText(3, query);
    statement.bindUInt64(4, static_cast<std::uint64_t>(limit));

    std::vector<UserSummary> users;
    while (statement.next())
    {
        users.emplace_back(
            statement.columnUInt64(0),
            statement.columnText(1),
            statement.columnText(2));
    }
    return users;
}
