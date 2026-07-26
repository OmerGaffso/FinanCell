#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/Transaction.h"

/** @brief Persistence contract for transactions and balances. */
class TransactionRepository
{
public:
    /** @brief Destroys the repository interface. */
    virtual ~TransactionRepository() = default;

    /** @brief Inserts a transaction. @param transaction Transaction data. @return Persisted transaction with generated ID. */
    virtual std::optional<Transaction> insertTransaction(const Transaction& transaction) = 0;
    /** @brief Finds a transaction. @param transactionId Transaction ID. @return Matching transaction, or empty. */
    virtual std::optional<Transaction> findTransactionById(uint64_t transactionId) const = 0;
    /** @brief Finds transactions in a cell. @param cellId Cell ID. @return Cell transactions. */
    virtual std::vector<Transaction> findTransactionsByCellId(uint64_t cellId) const = 0;
    /** @brief Finds transactions in a date range. @param cellId Cell ID. @param fromDate Inclusive start date. @param toDate Inclusive end date. @return Matching transactions. */
    virtual std::vector<Transaction> findTransactionsByDateRange(
        uint64_t cellId, const std::string& fromDate, const std::string& toDate) const = 0;
    /** @brief Updates a transaction. @param transaction Updated data. @return True when updated. */
    virtual bool updateTransaction(const Transaction& transaction) = 0;
    /** @brief Deletes a transaction. @param transactionId Transaction ID. @return True when deleted. */
    virtual bool deleteTransaction(uint64_t transactionId) = 0;
    /** @brief Calculates a cell balance. @param cellId Cell ID. @return Income minus expenses in minor units. */
    virtual int64_t calculateCellBalance(uint64_t cellId) const = 0;
};
