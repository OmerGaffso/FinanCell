#include "application/TransactionService.h"

#include <cctype>

#include "utils/StringUtils.h"

namespace
{
bool isLeapYear(int year)
{
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int year, int month)
{
    constexpr int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month - 1];
}
}

TransactionService::TransactionService(
    TransactionRepository& transactionRepository,
    CellRepository& cellRepository)
    : m_transactionRepository(transactionRepository),
      m_cellRepository(cellRepository)
{
}

std::optional<Transaction> TransactionService::addTransaction(
    uint64_t actingUserId,
    uint64_t cellId,
    TransactionType type,
    const std::string& description,
    int64_t amountInMinorUnits,
    const std::string& occurredAt,
    const std::string& category)
{
    const std::string trimmedDescription = StringUtils::trim(description);
    const std::string trimmedCategory = StringUtils::trim(category);
    if (!canWrite(actingUserId, cellId) ||
        !isValid(trimmedDescription, amountInMinorUnits, occurredAt, trimmedCategory))
    {
        return std::nullopt;
    }
    return m_transactionRepository.insertTransaction(Transaction(
        0, cellId, actingUserId, type, trimmedDescription, amountInMinorUnits,
        occurredAt, trimmedCategory));
}

bool TransactionService::editTransaction(
    uint64_t actingUserId,
    uint64_t cellId,
    uint64_t transactionId,
    TransactionType type,
    const std::string& description,
    int64_t amountInMinorUnits,
    const std::string& occurredAt,
    const std::string& category)
{
    const auto transaction = m_transactionRepository.findTransactionById(transactionId);
    const std::string trimmedDescription = StringUtils::trim(description);
    const std::string trimmedCategory = StringUtils::trim(category);
    const std::string effectiveCategory =
        trimmedCategory.empty() && transaction ? transaction->getCategory() : trimmedCategory;
    if (!transaction || transaction->getCellId() != cellId ||
        !isValid(trimmedDescription, amountInMinorUnits, occurredAt, effectiveCategory))
    {
        return false;
    }

    const auto membership = m_cellRepository.findMember(transaction->getCellId(), actingUserId);
    if (!membership || membership->role == CellRole::GUEST ||
        (membership->role != CellRole::OWNER && transaction->getUserId() != actingUserId))
    {
        return false;
    }

    return m_transactionRepository.updateTransaction(Transaction(
        transactionId,
        transaction->getCellId(),
        transaction->getUserId(),
        type,
        trimmedDescription,
        amountInMinorUnits,
        occurredAt.empty() ? transaction->getOccurredAt() : occurredAt,
        effectiveCategory));
}

bool TransactionService::deleteTransaction(
    uint64_t actingUserId,
    uint64_t cellId,
    uint64_t transactionId)
{
    const auto transaction = m_transactionRepository.findTransactionById(transactionId);
    if (!transaction || transaction->getCellId() != cellId) return false;
    const auto membership = m_cellRepository.findMember(transaction->getCellId(), actingUserId);
    if (!membership || membership->role == CellRole::GUEST ||
        (membership->role != CellRole::OWNER && transaction->getUserId() != actingUserId))
    {
        return false;
    }
    return m_transactionRepository.deleteTransaction(transactionId);
}

std::optional<std::vector<Transaction>> TransactionService::getTransactionsForCell(
    uint64_t actingUserId,
    uint64_t cellId,
    const std::string& fromDate,
    const std::string& toDate) const
{
    if (!canRead(actingUserId, cellId)) return std::nullopt;
    if (fromDate.empty() != toDate.empty() || !isDateValid(fromDate) ||
        !isDateValid(toDate) || (!fromDate.empty() && fromDate > toDate))
    {
        return std::nullopt;
    }
    if (!fromDate.empty())
        return m_transactionRepository.findTransactionsByDateRange(cellId, fromDate, toDate);
    return m_transactionRepository.findTransactionsByCellId(cellId);
}

std::optional<int64_t> TransactionService::getCellBalance(
    uint64_t actingUserId,
    uint64_t cellId) const
{
    if (!canRead(actingUserId, cellId)) return std::nullopt;
    return m_transactionRepository.calculateCellBalance(cellId);
}

bool TransactionService::canWrite(uint64_t userId, uint64_t cellId) const
{
    const auto membership = m_cellRepository.findMember(cellId, userId);
    return membership && membership->role != CellRole::GUEST;
}

bool TransactionService::canRead(uint64_t userId, uint64_t cellId) const
{
    return m_cellRepository.findMember(cellId, userId).has_value();
}

bool TransactionService::isValid(
    const std::string& description,
    int64_t amountInMinorUnits,
    const std::string& occurredAt,
    const std::string& category) const
{
    return !description.empty() && description.length() <= MAX_DESCRIPTION_LENGTH &&
           amountInMinorUnits > 0 && isDateValid(occurredAt) && isCategoryValid(category);
}

bool TransactionService::isDateValid(const std::string& date)
{
    if (date.empty()) return true;
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (std::size_t index = 0; index < date.size(); ++index)
    {
        if (index != 4 && index != 7 &&
            !std::isdigit(static_cast<unsigned char>(date[index])))
        {
            return false;
        }
    }

    const int year = std::stoi(date.substr(0, 4));
    const int month = std::stoi(date.substr(5, 2));
    const int day = std::stoi(date.substr(8, 2));
    return year >= 1 && month >= 1 && month <= 12 &&
           day >= 1 && day <= daysInMonth(year, month);
}

bool TransactionService::isCategoryValid(const std::string& category)
{
    const std::string trimmed = StringUtils::trim(category);
    return !trimmed.empty() && trimmed.length() <= 50;
}
