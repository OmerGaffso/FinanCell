#include "storage/sqlite/SQLiteCellRepository.h"

#include "storage/sqlite/SQLiteStatement.h"

namespace
{
FinancialCell readCell(SQLiteStatement& statement)
{
    return FinancialCell(
        statement.columnUInt64(0),
        statement.columnText(1),
        statement.columnText(2),
        statement.columnText(3),
        statement.columnUInt64(4));
}
}

SQLiteCellRepository::SQLiteCellRepository(SQLiteDatabase& database)
    : m_database(database)
{
}

std::optional<FinancialCell> SQLiteCellRepository::insertCell(
    const FinancialCell& cell)
{
    constexpr char sql[] =
        "INSERT INTO cells (name, description, currency, owner_user_id) "
        "VALUES (?, ?, ?, ?);";

    SQLiteStatement statement(m_database, sql);
    statement.bindText(1, cell.getCellName());
    statement.bindText(2, cell.getCellDescription());
    statement.bindText(3, cell.getUsesCurrency());
    statement.bindUInt64(4, cell.getOwnerId());
    statement.execute();

    return FinancialCell(
        m_database.lastInsertId(),
        cell.getCellName(),
        cell.getCellDescription(),
        cell.getUsesCurrency(),
        cell.getOwnerId());
}

std::optional<FinancialCell> SQLiteCellRepository::findCellById(
    std::uint64_t cellId) const
{
    constexpr char sql[] =
        "SELECT id, name, description, currency, owner_user_id "
        "FROM cells WHERE id = ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, cellId);

    if (!statement.next())
    {
        return std::nullopt;
    }

    return readCell(statement);
}

std::vector<FinancialCell> SQLiteCellRepository::findCellsByOwnerId(
    std::uint64_t ownerId) const
{
    constexpr char sql[] =
        "SELECT id, name, description, currency, owner_user_id "
        "FROM cells WHERE owner_user_id = ? ORDER BY id;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, ownerId);

    std::vector<FinancialCell> cells;
    while (statement.next())
    {
        cells.push_back(readCell(statement));
    }

    return cells;
}

std::vector<FinancialCell> SQLiteCellRepository::findAllCells() const
{
    constexpr char sql[] =
        "SELECT id, name, description, currency, owner_user_id "
        "FROM cells ORDER BY id;";

    SQLiteStatement statement(m_database, sql);

    std::vector<FinancialCell> cells;
    while (statement.next())
    {
        cells.push_back(readCell(statement));
    }

    return cells;
}
