#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "application/CellRepository.h"
#include "application/TransactionRepository.h"

/** @brief Income and expense totals for one category. */
struct CategoryReportLine
{
    /** @brief Category ID. */
    std::uint64_t categoryId;
    /** @brief Category name. */
    std::string categoryName;
    /** @brief Category income total in minor units. */
    std::int64_t incomeInMinorUnits{0};
    /** @brief Category expense total in minor units. */
    std::int64_t expensesInMinorUnits{0};
};

/** @brief Required monthly financial report for one cell. */
struct MonthlyReport
{
    /** @brief Report month in YYYY-MM format. */
    std::string month;
    /** @brief Total monthly income in minor units. */
    std::int64_t totalIncomeInMinorUnits{0};
    /** @brief Total monthly expenses in minor units. */
    std::int64_t totalExpensesInMinorUnits{0};
    /** @brief Income minus expenses in minor units. */
    std::int64_t balanceInMinorUnits{0};
    /** @brief Per-category report lines. */
    std::vector<CategoryReportLine> categories;
};

/** @brief Produces authorized monthly financial reports. */
class MonthlyReportService
{
public:
    /** @brief Creates the service. @param transactionRepository Transaction persistence. @param cellRepository Cell persistence. */
    MonthlyReportService(
        TransactionRepository& transactionRepository,
        CellRepository& cellRepository);

    /** @brief Generates a report. @param actingUserId Actor ID. @param cellId Cell ID. @param month YYYY-MM. @return Report, or empty for invalid/unauthorized requests. */
    std::optional<MonthlyReport> generate(
        std::uint64_t actingUserId,
        std::uint64_t cellId,
        const std::string& month) const;

private:
    TransactionRepository& m_transactionRepository;
    CellRepository& m_cellRepository;
};
