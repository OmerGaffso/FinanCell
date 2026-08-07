#include "storage/sqlite/SQLiteCategoryRepository.h"

#include "storage/sqlite/SQLiteStatement.h"

namespace
{
Category readCategory(SQLiteStatement& statement)
{
    return Category(
        statement.columnUInt64(0),
        statement.columnUInt64(1),
        statement.columnText(2),
        statement.columnInt64(3));
}
}

SQLiteCategoryRepository::SQLiteCategoryRepository(SQLiteDatabase& database)
    : m_database(database)
{
}

std::optional<Category> SQLiteCategoryRepository::insertCategory(const Category& category)
{
    SQLiteStatement statement(
        m_database,
        "INSERT INTO categories (cell_id, name) VALUES (?, ?);");
    statement.bindUInt64(1, category.getCellId());
    statement.bindText(2, category.getName());
    statement.execute();
    return findCategoryById(m_database.lastInsertId());
}

std::optional<Category> SQLiteCategoryRepository::findCategoryById(
    std::uint64_t categoryId) const
{
    SQLiteStatement statement(
        m_database,
        "SELECT id, cell_id, name, budget_minor FROM categories WHERE id = ?;");
    statement.bindUInt64(1, categoryId);
    if (!statement.next()) return std::nullopt;
    return readCategory(statement);
}

std::optional<Category> SQLiteCategoryRepository::findCategoryByName(
    std::uint64_t cellId,
    const std::string& name) const
{
    SQLiteStatement statement(
        m_database,
        "SELECT id, cell_id, name, budget_minor FROM categories WHERE cell_id = ? AND name = ?;");
    statement.bindUInt64(1, cellId);
    statement.bindText(2, name);
    if (!statement.next()) return std::nullopt;
    return readCategory(statement);
}

std::vector<Category> SQLiteCategoryRepository::findCategoriesByCellId(
    std::uint64_t cellId) const
{
    SQLiteStatement statement(
        m_database,
        "SELECT id, cell_id, name, budget_minor FROM categories "
        "WHERE cell_id = ? ORDER BY name COLLATE NOCASE, id;");
    statement.bindUInt64(1, cellId);
    std::vector<Category> categories;
    while (statement.next()) categories.push_back(readCategory(statement));
    return categories;
}

bool SQLiteCategoryRepository::updateCategoryBudget(
    std::uint64_t categoryId,
    std::int64_t amountInMinorUnits)
{
    SQLiteStatement statement(
        m_database,
        "UPDATE categories SET budget_minor = ? WHERE id = ?;");
    statement.bindInt64(1, amountInMinorUnits);
    statement.bindUInt64(2, categoryId);
    statement.execute();
    return m_database.changedRowCount() > 0;
}
