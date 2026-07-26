#include "application/TransactionService.h"

#include "utils/StringUtils.h"

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
    int64_t amountInMinorUnits)
{
    const std::string trimmedDescription = StringUtils::trim(description);
    if (!canWrite(actingUserId, cellId) || !isValid(trimmedDescription, amountInMinorUnits))
    {
        return std::nullopt;
    }
    return m_transactionRepository.insertTransaction(Transaction(
        0, cellId, actingUserId, type, trimmedDescription, amountInMinorUnits));
}

bool TransactionService::editTransaction(
    uint64_t actingUserId,
    uint64_t transactionId,
    TransactionType type,
    const std::string& description,
    int64_t amountInMinorUnits)
{
    const auto transaction = m_transactionRepository.findTransactionById(transactionId);
    const std::string trimmedDescription = StringUtils::trim(description);
    if (!transaction || !isValid(trimmedDescription, amountInMinorUnits)) return false;

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
        transaction->getOccurredAt()));
}

bool TransactionService::deleteTransaction(uint64_t actingUserId, uint64_t transactionId)
{
    const auto transaction = m_transactionRepository.findTransactionById(transactionId);
    if (!transaction) return false;
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
    uint64_t cellId) const
{
    if (!canRead(actingUserId, cellId)) return std::nullopt;
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
    int64_t amountInMinorUnits) const
{
    return !description.empty() && description.length() <= MAX_DESCRIPTION_LENGTH &&
           amountInMinorUnits > 0;
}
