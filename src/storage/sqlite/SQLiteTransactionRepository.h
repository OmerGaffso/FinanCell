#pragma once

#include "application/TransactionRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"

class SQLiteTransactionRepository final : public TransactionRepository
{
public:
    explicit SQLiteTransactionRepository(SQLiteDatabase& database);

    std::optional<Transaction> insertTransaction(const Transaction& transaction) override;
    std::optional<Transaction> findTransactionById(uint64_t transactionId) const override;
    std::vector<Transaction> findTransactionsByCellId(uint64_t cellId) const override;
    bool updateTransaction(const Transaction& transaction) override;
    bool deleteTransaction(uint64_t transactionId) override;
    int64_t calculateCellBalance(uint64_t cellId) const override;

private:
    SQLiteDatabase& m_database;
};
