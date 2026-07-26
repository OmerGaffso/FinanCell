#pragma once

#include <cstdint>
#include <string>
#include <utility>

/** @brief Direction of money represented by a transaction. */
enum class TransactionType
{
    INCOME,
    EXPENSE
};

/** @brief Immutable financial transaction stored in minor currency units. */
class Transaction
{
public:
    /**
     * @brief Constructs a transaction value.
     * @param transactionId Transaction ID.
     * @param cellId Containing cell ID.
     * @param userId Creating user ID.
     * @param type Income or expense.
     * @param description Transaction description.
     * @param amountInMinorUnits Positive amount in minor units.
     * @param occurredAt Date or timestamp.
     * @param category Reporting category.
     */
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

    /** @brief Returns the transaction ID. @return Transaction ID. */
    uint64_t getTransactionId() const { return m_transactionId; }
    /** @brief Returns the cell ID. @return Cell ID. */
    uint64_t getCellId() const { return m_cellId; }
    /** @brief Returns the creator ID. @return Creator user ID. */
    uint64_t getUserId() const { return m_userId; }
    /** @brief Returns the transaction type. @return Income or expense. */
    TransactionType getType() const { return m_type; }
    /** @brief Returns the description. @return Transaction description. */
    const std::string& getDescription() const { return m_description; }
    /** @brief Returns the amount. @return Positive amount in minor units. */
    int64_t getAmountInMinorUnits() const { return m_amountInMinorUnits; }
    /** @brief Returns the occurrence date. @return Stored date or timestamp. */
    const std::string& getOccurredAt() const { return m_occurredAt; }
    /** @brief Returns the category. @return Reporting category. */
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
