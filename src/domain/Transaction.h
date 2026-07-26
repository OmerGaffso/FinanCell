#pragma once

#include <cstdint>
#include <string>
#include <utility>

enum class TransactionType
{
    INCOME,
    EXPENSE
};

class Transaction
{
public:
    Transaction(
        uint64_t transactionId,
        uint64_t cellId,
        uint64_t userId,
        TransactionType type,
        std::string description,
        int64_t amountInMinorUnits,
        std::string occurredAt = "")
        : m_transactionId(transactionId),
          m_cellId(cellId),
          m_userId(userId),
          m_type(type),
          m_description(std::move(description)),
          m_amountInMinorUnits(amountInMinorUnits),
          m_occurredAt(std::move(occurredAt))
    {
    }

    uint64_t getTransactionId() const { return m_transactionId; }
    uint64_t getCellId() const { return m_cellId; }
    uint64_t getUserId() const { return m_userId; }
    TransactionType getType() const { return m_type; }
    const std::string& getDescription() const { return m_description; }
    int64_t getAmountInMinorUnits() const { return m_amountInMinorUnits; }
    const std::string& getOccurredAt() const { return m_occurredAt; }

private:
    uint64_t m_transactionId;
    uint64_t m_cellId;
    uint64_t m_userId;
    TransactionType m_type;
    std::string m_description;
    int64_t m_amountInMinorUnits;
    std::string m_occurredAt;
};
