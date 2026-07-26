#include "application/TransactionService.h"

#include "utils/StringUtils.h"
#include "utils/DateUtils.h"

TransactionService::TransactionService(
    TransactionRepository& transactionRepository,
    CellRepository& cellRepository,
    CategoryRepository& categoryRepository)
    : m_transactionRepository(transactionRepository),
      m_cellRepository(cellRepository),
      m_categoryRepository(categoryRepository)
{
}

std::optional<Transaction> TransactionService::addTransaction(
    uint64_t actingUserId,
    uint64_t cellId,
    TransactionType type,
    const std::string& description,
    int64_t amountInMinorUnits,
    const std::string& occurredAt,
    std::uint64_t categoryId)
{
    const std::string trimmedDescription = StringUtils::trim(description);
    if (!canWrite(actingUserId, cellId) ||
        !isValid(trimmedDescription, amountInMinorUnits, occurredAt) ||
        !categoryBelongsToCell(categoryId, cellId))
    {
        return std::nullopt;
    }
    return m_transactionRepository.insertTransaction(Transaction(
        0, cellId, actingUserId, type, trimmedDescription, amountInMinorUnits,
        occurredAt, categoryId));
}

bool TransactionService::editTransaction(
    uint64_t actingUserId,
    uint64_t cellId,
    uint64_t transactionId,
    TransactionType type,
    const std::string& description,
    int64_t amountInMinorUnits,
    const std::string& occurredAt,
    std::uint64_t categoryId)
{
    const auto transaction = m_transactionRepository.findTransactionById(transactionId);
    const std::string trimmedDescription = StringUtils::trim(description);
    const std::uint64_t effectiveCategoryId =
        categoryId == 0 && transaction ? transaction->getCategoryId() : categoryId;
    if (!transaction || transaction->getCellId() != cellId ||
        !isValid(trimmedDescription, amountInMinorUnits, occurredAt) ||
        !categoryBelongsToCell(effectiveCategoryId, cellId))
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
        effectiveCategoryId));
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
    const std::string& occurredAt) const
{
    return !description.empty() && description.length() <= MAX_DESCRIPTION_LENGTH &&
           amountInMinorUnits > 0 && isDateValid(occurredAt);
}

bool TransactionService::isDateValid(const std::string& date)
{
    return DateUtils::isIsoDateValid(date);
}

bool TransactionService::categoryBelongsToCell(
    std::uint64_t categoryId,
    std::uint64_t cellId) const
{
    const auto category = m_categoryRepository.findCategoryById(categoryId);
    return category && category->getCellId() == cellId;
}
