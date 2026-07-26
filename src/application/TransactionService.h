#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "application/CellRepository.h"
#include "application/CategoryRepository.h"
#include "application/TransactionRepository.h"

/** @brief Enforces transaction validation and role-based access rules. */
class TransactionService
{
public:
    /** @brief Maximum accepted transaction-description length. */
    static constexpr std::size_t MAX_DESCRIPTION_LENGTH = 200;

    /** @brief Creates the service. @param transactionRepository Transaction persistence. @param cellRepository Cell persistence. @param categoryRepository Category persistence. */
    TransactionService(
        TransactionRepository& transactionRepository,
        CellRepository& cellRepository,
        CategoryRepository& categoryRepository);

    /** @brief Adds a transaction. @param actingUserId Actor ID. @param cellId Cell ID. @param type Transaction type. @param description Description. @param amountInMinorUnits Positive minor-unit amount. @param occurredAt Date or timestamp. @param categoryId Category ID. @return Created transaction, or empty. */
    std::optional<Transaction> addTransaction(
        uint64_t actingUserId,
        uint64_t cellId,
        TransactionType type,
        const std::string& description,
        int64_t amountInMinorUnits,
        const std::string& occurredAt,
        std::uint64_t categoryId);
    /** @brief Edits a transaction. @param actingUserId Actor ID. @param cellId Selected cell ID. @param transactionId Transaction ID. @param type New type. @param description New description. @param amountInMinorUnits New amount. @param occurredAt New date, or empty to retain it. @param categoryId New category ID, or zero to retain it. @return True on success. */
    bool editTransaction(
        uint64_t actingUserId,
        uint64_t cellId,
        uint64_t transactionId,
        TransactionType type,
        const std::string& description,
        int64_t amountInMinorUnits,
        const std::string& occurredAt,
        std::uint64_t categoryId);
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
private:
    /** @brief Checks write access. @param userId User ID. @param cellId Cell ID. @return True when writable. */
    bool canWrite(uint64_t userId, uint64_t cellId) const;
    /** @brief Checks read access. @param userId User ID. @param cellId Cell ID. @return True when readable. */
    bool canRead(uint64_t userId, uint64_t cellId) const;
    /** @brief Validates transaction fields. @param description Description. @param amountInMinorUnits Amount. @param occurredAt Date. @return True when valid. */
    bool isValid(
        const std::string& description,
        int64_t amountInMinorUnits,
        const std::string& occurredAt) const;
    /** @brief Checks that a category belongs to a cell. */
    bool categoryBelongsToCell(std::uint64_t categoryId, std::uint64_t cellId) const;

    TransactionRepository& m_transactionRepository;
    CellRepository& m_cellRepository;
    CategoryRepository& m_categoryRepository;
};
