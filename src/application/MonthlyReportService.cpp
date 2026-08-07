#include "application/MonthlyReportService.h"

#include <limits>
#include <map>
#include <stdexcept>

#include "utils/DateUtils.h"

namespace
{
void checkedAdd(std::int64_t& total, std::int64_t value)
{
    if (value > 0 && total > std::numeric_limits<std::int64_t>::max() - value)
        throw std::overflow_error("Monthly report total is too large.");
    if (value < 0 && total < std::numeric_limits<std::int64_t>::min() - value)
        throw std::overflow_error("Monthly report total is too small.");
    total += value;
}

std::int64_t checkedSubtract(std::int64_t left, std::int64_t right)
{
    if (right > 0 && left < std::numeric_limits<std::int64_t>::min() + right)
        throw std::overflow_error("Monthly report balance is too small.");
    if (right < 0 && left > std::numeric_limits<std::int64_t>::max() + right)
        throw std::overflow_error("Monthly report balance is too large.");
    return left - right;
}
}

MonthlyReportService::MonthlyReportService(
    TransactionRepository& transactionRepository,
    CellRepository& cellRepository,
    CategoryRepository& categoryRepository)
    : m_transactionRepository(transactionRepository),
      m_cellRepository(cellRepository),
      m_categoryRepository(categoryRepository)
{
}

std::optional<MonthlyReport> MonthlyReportService::generate(
    std::uint64_t actingUserId,
    std::uint64_t cellId,
    const std::string& month) const
{
    const auto range = DateUtils::monthRange(month);
    if (!range || !m_cellRepository.findMember(cellId, actingUserId)) return std::nullopt;

    MonthlyReport report;
    report.month = month;
    std::map<std::uint64_t, CategoryReportLine> lines;
    for (const Category& category : m_categoryRepository.findCategoriesByCellId(cellId))
    {
        if (category.getMonthlyBudgetInMinorUnits() <= 0) continue;
        auto& line = lines[category.getCategoryId()];
        line.categoryId = category.getCategoryId();
        line.categoryName = category.getName();
        line.monthlyBudgetInMinorUnits = category.getMonthlyBudgetInMinorUnits();
    }
    for (const Transaction& transaction : m_transactionRepository.findTransactionsByDateRange(
             cellId, range->first, range->second))
    {
        auto& line = lines[transaction.getCategoryId()];
        line.categoryId = transaction.getCategoryId();
        line.categoryName = transaction.getCategoryName();
        if (transaction.getType() == TransactionType::INCOME)
        {
            checkedAdd(report.totalIncomeInMinorUnits, transaction.getAmountInMinorUnits());
            checkedAdd(line.incomeInMinorUnits, transaction.getAmountInMinorUnits());
        }
        else
        {
            checkedAdd(report.totalExpensesInMinorUnits, transaction.getAmountInMinorUnits());
            checkedAdd(line.expensesInMinorUnits, transaction.getAmountInMinorUnits());
        }
    }

    report.balanceInMinorUnits = checkedSubtract(
        report.totalIncomeInMinorUnits, report.totalExpensesInMinorUnits);
    for (auto& [id, line] : lines)
    {
        static_cast<void>(id);
        if (line.monthlyBudgetInMinorUnits > 0)
        {
            line.remainingBudgetInMinorUnits = checkedSubtract(
                line.monthlyBudgetInMinorUnits, line.expensesInMinorUnits);
            line.overBudget = line.expensesInMinorUnits > line.monthlyBudgetInMinorUnits;
        }
        report.categories.push_back(std::move(line));
    }
    return report;
}
