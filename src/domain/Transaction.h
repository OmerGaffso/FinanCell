#pragma once

#include <cstdint>
#include <string>

enum class TransactionType
{
    INCOME,
    EXPENSE
};

class Transaction
{
    public:
        Transaction(uint64_t transactionId, uint64_t cellId, uint64_t userId, TransactionType type, const std::string& description, int64_t amountInMinorUnits)
            : m_transactionId(transactionId), m_cellId(cellId), m_userId(userId), m_type(type), m_description(description), m_amountInMinorUnits(amountInMinorUnits) {}
    
    private:
    uint64_t m_transactionId;
    uint64_t m_cellId;
    uint64_t m_userId;
    TransactionType m_type;
    std::string m_description;
    int64_t m_amountInMinorUnits; // Amount in the smallest currency unit (e.g., cents or agorot)
};
