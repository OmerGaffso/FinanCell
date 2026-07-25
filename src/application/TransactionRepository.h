#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/Transaction.h"

class TransactionRepository
{
public:
    virtual ~TransactionRepository() = default;

    virtual bool insertTransaction(const Transaction& transaction) = 0;
    virtual std::optional<Transaction> findTransactionById(uint64_t transactionId) const = 0;
    virtual std::vector<Transaction> findAllTransactions() const = 0;
};
