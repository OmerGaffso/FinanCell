#include "storage/sqlite/SQLiteCellRepository.h"

#include <stdexcept>
#include <string>

#include "storage/sqlite/SQLiteStatement.h"
#include "application/PersistenceError.h"

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

const char* cellRoleToText(CellRole role)
{
    switch (role)
    {
        case CellRole::MANAGER:
            return "MANAGER";
        case CellRole::MEMBER:
            return "MEMBER";
        case CellRole::GUEST:
            return "GUEST";
    }

    throw std::invalid_argument("Unknown cell role.");
}

CellRole cellRoleFromText(const std::string& role)
{
    if (role == "MANAGER")
    {
        return CellRole::MANAGER;
    }
    if (role == "MEMBER")
    {
        return CellRole::MEMBER;
    }
    if (role == "GUEST")
    {
        return CellRole::GUEST;
    }

    throw PersistenceError("Database contains an unknown cell role: " + role);
}

CellMember readMember(SQLiteStatement& statement)
{
    return CellMember{
        statement.columnUInt64(1),
        statement.columnUInt64(0),
        cellRoleFromText(statement.columnText(2))};
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
    statement.bindText(3, cell.getCurrency());
    statement.bindUInt64(4, cell.getCreatorId());
    statement.execute();

    return FinancialCell(
        m_database.lastInsertId(),
        cell.getCellName(),
        cell.getCellDescription(),
        cell.getCurrency(),
        cell.getCreatorId());
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

bool SQLiteCellRepository::updateCell(const FinancialCell& cell)
{
    constexpr char sql[] =
        "UPDATE cells SET name = ?, description = ?, currency = ? "
        "WHERE id = ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindText(1, cell.getCellName());
    statement.bindText(2, cell.getCellDescription());
    statement.bindText(3, cell.getCurrency());
    statement.bindUInt64(4, cell.getCellId());
    statement.execute();
    return m_database.changedRowCount() > 0;
}

bool SQLiteCellRepository::deleteCell(std::uint64_t cellId)
{
    SQLiteStatement statement(m_database, "DELETE FROM cells WHERE id = ?;");
    statement.bindUInt64(1, cellId);
    statement.execute();
    return m_database.changedRowCount() > 0;
}

std::vector<FinancialCell> SQLiteCellRepository::findCellsByCreatorId(
    std::uint64_t creatorId) const
{
    constexpr char sql[] =
        "SELECT id, name, description, currency, owner_user_id "
        "FROM cells WHERE owner_user_id = ? ORDER BY id;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, creatorId);

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

bool SQLiteCellRepository::insertMember(const CellMember& member)
{
    constexpr char sql[] =
        "INSERT INTO cell_members (cell_id, user_id, role) "
        "VALUES (?, ?, ?);";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, member.cellId);
    statement.bindUInt64(2, member.userId);
    statement.bindText(3, cellRoleToText(member.role));
    statement.execute();
    return true;
}

std::optional<CellMember> SQLiteCellRepository::findMember(
    std::uint64_t cellId,
    std::uint64_t userId) const
{
    constexpr char sql[] =
        "SELECT cell_id, user_id, role "
        "FROM cell_members WHERE cell_id = ? AND user_id = ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, cellId);
    statement.bindUInt64(2, userId);

    if (!statement.next())
    {
        return std::nullopt;
    }

    return readMember(statement);
}

std::vector<CellMember> SQLiteCellRepository::findMembersByCellId(
    std::uint64_t cellId) const
{
    constexpr char sql[] =
        "SELECT cell_id, user_id, role FROM cell_members "
        "WHERE cell_id = ? ORDER BY user_id;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, cellId);

    std::vector<CellMember> members;
    while (statement.next())
    {
        members.push_back(readMember(statement));
    }

    return members;
}

std::vector<FinancialCell> SQLiteCellRepository::findCellsByUserId(
    std::uint64_t userId) const
{
    constexpr char sql[] =
        "SELECT c.id, c.name, c.description, c.currency, c.owner_user_id "
        "FROM cells c "
        "INNER JOIN cell_members cm ON cm.cell_id = c.id "
        "WHERE cm.user_id = ? ORDER BY c.id;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, userId);

    std::vector<FinancialCell> cells;
    while (statement.next())
    {
        cells.push_back(readCell(statement));
    }

    return cells;
}

bool SQLiteCellRepository::updateMemberRole(
    std::uint64_t cellId,
    std::uint64_t userId,
    CellRole role)
{
    constexpr char sql[] =
        "UPDATE cell_members SET role = ? "
        "WHERE cell_id = ? AND user_id = ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindText(1, cellRoleToText(role));
    statement.bindUInt64(2, cellId);
    statement.bindUInt64(3, userId);
    statement.execute();
    return m_database.changedRowCount() > 0;
}

bool SQLiteCellRepository::deleteMember(
    std::uint64_t cellId,
    std::uint64_t userId)
{
    constexpr char sql[] =
        "DELETE FROM cell_members WHERE cell_id = ? AND user_id = ?;";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, cellId);
    statement.bindUInt64(2, userId);
    statement.execute();
    return m_database.changedRowCount() > 0;
}
