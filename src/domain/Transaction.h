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
     * @param categoryId Reporting category ID.
     * @param categoryName Reporting category name loaded from persistence.
     */
    Transaction(
        uint64_t transactionId,
        uint64_t cellId,
        uint64_t userId,
        TransactionType type,
        std::string description,
        int64_t amountInMinorUnits,
        std::string occurredAt = "",
        uint64_t categoryId = 0,
        std::string categoryName = "")
        : m_transactionId(transactionId),
          m_cellId(cellId),
          m_userId(userId),
          m_type(type),
          m_description(std::move(description)),
          m_amountInMinorUnits(amountInMinorUnits),
          m_occurredAt(std::move(occurredAt)),
          m_categoryId(categoryId),
          m_categoryName(std::move(categoryName))
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
    /** @brief Returns the category ID. @return Reporting category ID. */
    uint64_t getCategoryId() const { return m_categoryId; }
    /** @brief Returns the category name. @return Reporting category name. */
    const std::string& getCategoryName() const { return m_categoryName; }

private:
    uint64_t m_transactionId;
    uint64_t m_cellId;
    uint64_t m_userId;
    TransactionType m_type;
    std::string m_description;
    int64_t m_amountInMinorUnits;
    std::string m_occurredAt;
    uint64_t m_categoryId;
    std::string m_categoryName;
};
