#pragma once

#include <cstdint>
#include <string>
#include <utility>

/** Direction of money represented by a transaction. */
enum class TransactionType
{
    INCOME,
    EXPENSE
};

/** Immutable financial transaction stored in minor currency units. */
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
        std::string occurredAt = "",
        std::string category = "General")
        : m_transactionId(transactionId),
          m_cellId(cellId),
          m_userId(userId),
          m_type(type),
          m_description(std::move(description)),
          m_amountInMinorUnits(amountInMinorUnits),
          m_occurredAt(std::move(occurredAt)),
          m_category(std::move(category))
    {
    }

    uint64_t getTransactionId() const { return m_transactionId; }
    uint64_t getCellId() const { return m_cellId; }
    uint64_t getUserId() const { return m_userId; }
    TransactionType getType() const { return m_type; }
    const std::string& getDescription() const { return m_description; }
    int64_t getAmountInMinorUnits() const { return m_amountInMinorUnits; }
    const std::string& getOccurredAt() const { return m_occurredAt; }
    const std::string& getCategory() const { return m_category; }

private:
    uint64_t m_transactionId;
    uint64_t m_cellId;
    uint64_t m_userId;
    TransactionType m_type;
    std::string m_description;
    int64_t m_amountInMinorUnits;
    std::string m_occurredAt;
    std::string m_category;
};
