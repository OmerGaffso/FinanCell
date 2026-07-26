#pragma once

#include "application/TransactionRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"

/** @brief SQLite implementation of transaction persistence. */
class SQLiteTransactionRepository final : public TransactionRepository
{
public:
    /** @brief Creates the repository. @param database Shared database connection. */
    explicit SQLiteTransactionRepository(SQLiteDatabase& database);

    /** @copydoc TransactionRepository::insertTransaction */
    std::optional<Transaction> insertTransaction(const Transaction& transaction) override;
    /** @copydoc TransactionRepository::findTransactionById */
    std::optional<Transaction> findTransactionById(uint64_t transactionId) const override;
    /** @copydoc TransactionRepository::findTransactionsByCellId */
    std::vector<Transaction> findTransactionsByCellId(uint64_t cellId) const override;
    /** @copydoc TransactionRepository::findTransactionsByDateRange */
    std::vector<Transaction> findTransactionsByDateRange(
        uint64_t cellId, const std::string& fromDate, const std::string& toDate) const override;
    /** @copydoc TransactionRepository::updateTransaction */
    bool updateTransaction(const Transaction& transaction) override;
    /** @copydoc TransactionRepository::deleteTransaction */
    bool deleteTransaction(uint64_t transactionId) override;
    /** @copydoc TransactionRepository::calculateCellBalance */
    int64_t calculateCellBalance(uint64_t cellId) const override;

private:
    SQLiteDatabase& m_database;
};
