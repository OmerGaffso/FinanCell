#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/Transaction.h"

/** Persistence contract for financial transactions and balances. */
class TransactionRepository
{
public:
    virtual ~TransactionRepository() = default;

    virtual std::optional<Transaction> insertTransaction(const Transaction& transaction) = 0;
    virtual std::optional<Transaction> findTransactionById(uint64_t transactionId) const = 0;
    virtual std::vector<Transaction> findTransactionsByCellId(uint64_t cellId) const = 0;
    virtual std::vector<Transaction> findTransactionsByDateRange(
        uint64_t cellId, const std::string& fromDate, const std::string& toDate) const = 0;
    virtual bool updateTransaction(const Transaction& transaction) = 0;
    virtual bool deleteTransaction(uint64_t transactionId) = 0;
    virtual int64_t calculateCellBalance(uint64_t cellId) const = 0;
};
