#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "application/CellRepository.h"
#include "application/TransactionRepository.h"

/** @brief Enforces transaction validation and role-based access rules. */
class TransactionService
{
public:
    /** @brief Maximum accepted transaction-description length. */
    static constexpr std::size_t MAX_DESCRIPTION_LENGTH = 200;

    /** @brief Creates the service. @param transactionRepository Transaction persistence. @param cellRepository Cell persistence. */
    TransactionService(
        TransactionRepository& transactionRepository,
        CellRepository& cellRepository);

    /** @brief Adds a transaction. @param actingUserId Actor ID. @param cellId Cell ID. @param type Transaction type. @param description Description. @param amountInMinorUnits Positive minor-unit amount. @param occurredAt Date or timestamp. @param category Category. @return Created transaction, or empty. */
    std::optional<Transaction> addTransaction(
        uint64_t actingUserId,
        uint64_t cellId,
        TransactionType type,
        const std::string& description,
        int64_t amountInMinorUnits,
        const std::string& occurredAt = "",
        const std::string& category = "General");
    /** @brief Edits a transaction. @param actingUserId Actor ID. @param cellId Selected cell ID. @param transactionId Transaction ID. @param type New type. @param description New description. @param amountInMinorUnits New amount. @param occurredAt New date, or empty to retain it. @param category New category, or empty to retain it. @return True on success. */
    bool editTransaction(
        uint64_t actingUserId,
        uint64_t cellId,
        uint64_t transactionId,
        TransactionType type,
        const std::string& description,
        int64_t amountInMinorUnits,
        const std::string& occurredAt,
        const std::string& category);
    /** @brief Deletes a transaction. @param actingUserId Actor ID. @param cellId Selected cell ID. @param transactionId Transaction ID. @return True on success. */
    bool deleteTransaction(uint64_t actingUserId, uint64_t cellId, uint64_t transactionId);
    /** @brief Returns authorized transactions. @param actingUserId Actor ID. @param cellId Cell ID. @param fromDate Optional start date. @param toDate Optional end date. @return Transactions, or empty optional when unauthorized. */
    std::optional<std::vector<Transaction>> getTransactionsForCell(
        uint64_t actingUserId,
        uint64_t cellId,
        const std::string& fromDate = "",
        const std::string& toDate = "") const;
    /** @brief Returns an authorized cell balance. @param actingUserId Actor ID. @param cellId Cell ID. @return Balance, or empty when unauthorized. */
    std::optional<int64_t> getCellBalance(uint64_t actingUserId, uint64_t cellId) const;

    /** @brief Validates an ISO calendar date. @param date Empty text or YYYY-MM-DD. @return True when valid. */
    static bool isDateValid(const std::string& date);
    /** @brief Validates a transaction category. @param category Category text. @return True when valid. */
    static bool isCategoryValid(const std::string& category);

private:
    /** @brief Checks write access. @param userId User ID. @param cellId Cell ID. @return True when writable. */
    bool canWrite(uint64_t userId, uint64_t cellId) const;
    /** @brief Checks read access. @param userId User ID. @param cellId Cell ID. @return True when readable. */
    bool canRead(uint64_t userId, uint64_t cellId) const;
    /** @brief Validates transaction fields. @param description Description. @param amountInMinorUnits Amount. @param occurredAt Date. @param category Category. @return True when valid. */
    bool isValid(
        const std::string& description,
        int64_t amountInMinorUnits,
        const std::string& occurredAt,
        const std::string& category) const;

    TransactionRepository& m_transactionRepository;
    CellRepository& m_cellRepository;
};
