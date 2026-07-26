#include "storage/sqlite/SQLiteTransactionRepository.h"

#include <stdexcept>

#include "storage/sqlite/SQLiteStatement.h"

namespace
{
const char* transactionTypeToText(TransactionType type)
{
    switch (type)
    {
        case TransactionType::INCOME:
            return "INCOME";
        case TransactionType::EXPENSE:
            return "EXPENSE";
    }
    throw std::invalid_argument("Unknown transaction type.");
}

TransactionType transactionTypeFromText(const std::string& type)
{
    if (type == "INCOME") return TransactionType::INCOME;
    if (type == "EXPENSE") return TransactionType::EXPENSE;
    throw std::runtime_error("Database contains an unknown transaction type: " + type);
}

Transaction readTransaction(SQLiteStatement& statement)
{
    return Transaction(
        statement.columnUInt64(0),
        statement.columnUInt64(1),
        statement.columnUInt64(2),
        transactionTypeFromText(statement.columnText(3)),
        statement.columnText(4),
        statement.columnInt64(5),
        statement.columnText(6));
}
}

SQLiteTransactionRepository::SQLiteTransactionRepository(SQLiteDatabase& database)
    : m_database(database)
{
}

std::optional<Transaction> SQLiteTransactionRepository::insertTransaction(
    const Transaction& transaction)
{
    constexpr char sql[] =
        "INSERT INTO transactions "
        "(cell_id, created_by_user_id, type, description, amount_minor) "
        "VALUES (?, ?, ?, ?, ?);";

    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, transaction.getCellId());
    statement.bindUInt64(2, transaction.getUserId());
    statement.bindText(3, transactionTypeToText(transaction.getType()));
    statement.bindText(4, transaction.getDescription());
    statement.bindInt64(5, transaction.getAmountInMinorUnits());
    statement.execute();
    return findTransactionById(m_database.lastInsertId());
}

std::optional<Transaction> SQLiteTransactionRepository::findTransactionById(
    uint64_t transactionId) const
{
    constexpr char sql[] =
        "SELECT id, cell_id, created_by_user_id, type, description, "
        "amount_minor, occurred_at FROM transactions WHERE id = ?;";
    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, transactionId);
    if (!statement.next()) return std::nullopt;
    return readTransaction(statement);
}

std::vector<Transaction> SQLiteTransactionRepository::findTransactionsByCellId(
    uint64_t cellId) const
{
    constexpr char sql[] =
        "SELECT id, cell_id, created_by_user_id, type, description, "
        "amount_minor, occurred_at FROM transactions "
        "WHERE cell_id = ? ORDER BY occurred_at, id;";
    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, cellId);
    std::vector<Transaction> transactions;
    while (statement.next()) transactions.push_back(readTransaction(statement));
    return transactions;
}

bool SQLiteTransactionRepository::updateTransaction(const Transaction& transaction)
{
    constexpr char sql[] =
        "UPDATE transactions SET type = ?, description = ?, amount_minor = ?, "
        "updated_at = CURRENT_TIMESTAMP WHERE id = ?;";
    SQLiteStatement statement(m_database, sql);
    statement.bindText(1, transactionTypeToText(transaction.getType()));
    statement.bindText(2, transaction.getDescription());
    statement.bindInt64(3, transaction.getAmountInMinorUnits());
    statement.bindUInt64(4, transaction.getTransactionId());
    statement.execute();
    return m_database.changedRowCount() > 0;
}

bool SQLiteTransactionRepository::deleteTransaction(uint64_t transactionId)
{
    SQLiteStatement statement(m_database, "DELETE FROM transactions WHERE id = ?;");
    statement.bindUInt64(1, transactionId);
    statement.execute();
    return m_database.changedRowCount() > 0;
}

int64_t SQLiteTransactionRepository::calculateCellBalance(uint64_t cellId) const
{
    constexpr char sql[] =
        "SELECT COALESCE(SUM(CASE type WHEN 'INCOME' THEN amount_minor "
        "WHEN 'EXPENSE' THEN -amount_minor END), 0) "
        "FROM transactions WHERE cell_id = ?;";
    SQLiteStatement statement(m_database, sql);
    statement.bindUInt64(1, cellId);
    if (!statement.next()) return 0;
    return statement.columnInt64(0);
}
