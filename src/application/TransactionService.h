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
    /** @brief Edits a transaction. @param actingUserId Actor ID. @param transactionId Transaction ID. @param type New type. @param description New description. @param amountInMinorUnits New amount. @return True on success. */
    bool editTransaction(
        uint64_t actingUserId,
        uint64_t transactionId,
        TransactionType type,
        const std::string& description,
        int64_t amountInMinorUnits);
    /** @brief Deletes a transaction. @param actingUserId Actor ID. @param transactionId Transaction ID. @return True on success. */
    bool deleteTransaction(uint64_t actingUserId, uint64_t transactionId);
    /** @brief Returns authorized transactions. @param actingUserId Actor ID. @param cellId Cell ID. @param fromDate Optional start date. @param toDate Optional end date. @return Transactions, or empty optional when unauthorized. */
    std::optional<std::vector<Transaction>> getTransactionsForCell(
        uint64_t actingUserId,
        uint64_t cellId,
        const std::string& fromDate = "",
        const std::string& toDate = "") const;
    /** @brief Returns an authorized cell balance. @param actingUserId Actor ID. @param cellId Cell ID. @return Balance, or empty when unauthorized. */
    std::optional<int64_t> getCellBalance(uint64_t actingUserId, uint64_t cellId) const;

private:
    /** @brief Checks write access. @param userId User ID. @param cellId Cell ID. @return True when writable. */
    bool canWrite(uint64_t userId, uint64_t cellId) const;
    /** @brief Checks read access. @param userId User ID. @param cellId Cell ID. @return True when readable. */
    bool canRead(uint64_t userId, uint64_t cellId) const;
    /** @brief Validates transaction fields. @param description Description. @param amountInMinorUnits Amount. @return True when valid. */
    bool isValid(const std::string& description, int64_t amountInMinorUnits) const;

    TransactionRepository& m_transactionRepository;
    CellRepository& m_cellRepository;
};
