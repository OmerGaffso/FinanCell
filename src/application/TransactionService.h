#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "application/CellRepository.h"
#include "application/TransactionRepository.h"

/** Enforces transaction validation and role-based access rules. */
class TransactionService
{
public:
    static constexpr std::size_t MAX_DESCRIPTION_LENGTH = 200;

    TransactionService(
        TransactionRepository& transactionRepository,
        CellRepository& cellRepository);

    std::optional<Transaction> addTransaction(
        uint64_t actingUserId,
        uint64_t cellId,
        TransactionType type,
        const std::string& description,
        int64_t amountInMinorUnits,
        const std::string& occurredAt = "",
        const std::string& category = "General");
    bool editTransaction(
        uint64_t actingUserId,
        uint64_t transactionId,
        TransactionType type,
        const std::string& description,
        int64_t amountInMinorUnits);
    bool deleteTransaction(uint64_t actingUserId, uint64_t transactionId);
    std::optional<std::vector<Transaction>> getTransactionsForCell(
        uint64_t actingUserId,
        uint64_t cellId,
        const std::string& fromDate = "",
        const std::string& toDate = "") const;
    std::optional<int64_t> getCellBalance(uint64_t actingUserId, uint64_t cellId) const;

private:
    bool canWrite(uint64_t userId, uint64_t cellId) const;
    bool canRead(uint64_t userId, uint64_t cellId) const;
    bool isValid(const std::string& description, int64_t amountInMinorUnits) const;

    TransactionRepository& m_transactionRepository;
    CellRepository& m_cellRepository;
};
