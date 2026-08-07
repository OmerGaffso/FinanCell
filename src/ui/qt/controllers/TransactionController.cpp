#include "ui/qt/controllers/TransactionController.h"

#include <QDebug>

#include <cstdint>
#include <exception>
#include <iomanip>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

#include "application/CellService.h"
#include "application/TransactionService.h"
#include "ui/qt/session/SessionState.h"

namespace
{
QString formatMoney(std::int64_t amount, const std::string& currency)
{
    const bool negative = amount < 0;
    const std::uint64_t magnitude = negative
        ? static_cast<std::uint64_t>(-(amount + 1)) + 1
        : static_cast<std::uint64_t>(amount);
    std::ostringstream output;
    if (negative) output << '-';
    output << magnitude / 100 << '.' << std::setw(2) << std::setfill('0')
           << magnitude % 100 << ' ' << currency;
    return QString::fromStdString(output.str());
}

QString formatShortDate(const std::string& value)
{
    if (value.size() < 10 || value[4] != '-' || value[7] != '-')
        return QString::fromStdString(value);
    return QStringLiteral("%1/%2/%3")
        .arg(QString::fromStdString(value.substr(8, 2)))
        .arg(QString::fromStdString(value.substr(5, 2)))
        .arg(QString::fromStdString(value.substr(2, 2)));
}

QString amountInput(std::int64_t amount)
{
    std::ostringstream output;
    output << amount / 100 << '.' << std::setw(2) << std::setfill('0')
           << amount % 100;
    return QString::fromStdString(output.str());
}

std::optional<std::int64_t> parseAmount(const QString& input)
{
    const std::string text = input.trimmed().toStdString();
    if (text.empty()) return std::nullopt;
    const std::size_t dot = text.find('.');
    if (dot != std::string::npos && text.find('.', dot + 1) != std::string::npos)
        return std::nullopt;
    const std::string wholeText = dot == std::string::npos ? text : text.substr(0, dot);
    const std::string fractionText = dot == std::string::npos ? "" : text.substr(dot + 1);
    if (wholeText.empty() || fractionText.size() > 2) return std::nullopt;
    for (char value : wholeText)
        if (value < '0' || value > '9') return std::nullopt;
    for (char value : fractionText)
        if (value < '0' || value > '9') return std::nullopt;

    try
    {
        const std::uint64_t whole = std::stoull(wholeText);
        const std::uint64_t fraction = fractionText.empty()
            ? 0
            : static_cast<std::uint64_t>(std::stoul(fractionText)) *
                  (fractionText.size() == 1 ? 10 : 1);
        const auto maximum = static_cast<std::uint64_t>(
            std::numeric_limits<std::int64_t>::max());
        if (whole > (maximum - fraction) / 100) return std::nullopt;
        const std::uint64_t total = whole * 100 + fraction;
        if (total == 0) return std::nullopt;
        return static_cast<std::int64_t>(total);
    }
    catch (const std::exception&)
    {
        return std::nullopt;
    }
}

std::optional<TransactionType> parseType(const QString& type)
{
    const QString normalized = type.trimmed().toUpper();
    if (normalized == QStringLiteral("INCOME")) return TransactionType::INCOME;
    if (normalized == QStringLiteral("EXPENSE")) return TransactionType::EXPENSE;
    return std::nullopt;
}
}

TransactionController::TransactionController(
    TransactionService& transactionService,
    CellService& cellService,
    SessionState& session,
    QObject* parent)
    : QObject(parent),
      m_transactionService(transactionService),
      m_cellService(cellService),
      m_session(session)
{
    connect(&m_session, &SessionState::sessionChanged, this, [this]()
    {
        clearTransactions();
        clearSelection();
        clearError();
    });
}

QVariantList TransactionController::transactions() const { return m_transactions; }
QVariantMap TransactionController::selectedTransaction() const { return m_selectedTransaction; }
bool TransactionController::hasSelectedTransaction() const { return !m_selectedTransaction.isEmpty(); }
bool TransactionController::canWrite() const { return m_canWrite; }
QString TransactionController::balanceText() const { return m_balanceText; }
QString TransactionController::errorMessage() const { return m_errorMessage; }

bool TransactionController::loadTransactions(
    qulonglong cellId,
    const QString& fromDate,
    const QString& toDate)
{
    clearError();
    try
    {
        const auto cell = m_cellService.getCellForUser(m_session.userId(), cellId);
        const auto transactions = m_transactionService.getTransactionsForCell(
            m_session.userId(), cellId, fromDate.toStdString(), toDate.toStdString());
        const auto balance = m_transactionService.getCellBalance(m_session.userId(), cellId);
        if (!cell || !transactions || !balance)
        {
            clearTransactions();
            setErrorMessage(QStringLiteral(
                "Check the cell and enter a complete valid date range."));
            return false;
        }

        bool canWrite = false;
        bool isOwner = false;
        for (const auto& member : m_cellService.getCellMembers(m_session.userId(), cellId))
        {
            if (member.userId == m_session.userId())
            {
                canWrite = member.role != CellRole::GUEST;
                isOwner = member.role == CellRole::OWNER;
            }
        }

        QVariantList values;
        for (const auto& transaction : *transactions)
        {
            QVariantMap value;
            value.insert(QStringLiteral("transactionId"),
                         QVariant::fromValue<qulonglong>(transaction.getTransactionId()));
            value.insert(QStringLiteral("creatorUserId"),
                         QVariant::fromValue<qulonglong>(transaction.getUserId()));
            value.insert(QStringLiteral("type"), transaction.getType() == TransactionType::INCOME
                ? QStringLiteral("INCOME") : QStringLiteral("EXPENSE"));
            value.insert(QStringLiteral("description"),
                         QString::fromStdString(transaction.getDescription()));
            value.insert(QStringLiteral("amountText"),
                         formatMoney(transaction.getAmountInMinorUnits(), cell->getCurrency()));
            value.insert(QStringLiteral("amountInput"),
                         amountInput(transaction.getAmountInMinorUnits()));
            value.insert(QStringLiteral("occurredAt"),
                         QString::fromStdString(transaction.getOccurredAt()));
            value.insert(QStringLiteral("dateText"),
                         formatShortDate(transaction.getOccurredAt()));
            value.insert(QStringLiteral("dateInput"),
                         QString::fromStdString(transaction.getOccurredAt().substr(0, 10)));
            value.insert(QStringLiteral("categoryId"),
                         QVariant::fromValue<qulonglong>(transaction.getCategoryId()));
            value.insert(QStringLiteral("categoryName"),
                         QString::fromStdString(transaction.getCategoryName()));
            value.insert(QStringLiteral("editable"),
                         canWrite && (isOwner || transaction.getUserId() == m_session.userId()));
            values.append(value);
        }
        m_transactions = std::move(values);
        m_canWrite = canWrite;
        m_balanceText = formatMoney(*balance, cell->getCurrency());
        emit transactionsChanged();
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Transaction loading failed:" << error.what();
        clearTransactions();
        setErrorMessage(QStringLiteral("Transactions could not be loaded."));
        return false;
    }
}

bool TransactionController::selectTransaction(qulonglong transactionId)
{
    clearError();
    for (const QVariant& value : m_transactions)
    {
        const QVariantMap transaction = value.toMap();
        if (transaction.value(QStringLiteral("transactionId")).toULongLong() == transactionId &&
            transaction.value(QStringLiteral("editable")).toBool())
        {
            m_selectedTransaction = transaction;
            emit selectedTransactionChanged();
            return true;
        }
    }
    setErrorMessage(QStringLiteral("This transaction cannot be edited."));
    return false;
}

void TransactionController::clearSelection()
{
    if (m_selectedTransaction.isEmpty()) return;
    m_selectedTransaction.clear();
    emit selectedTransactionChanged();
}

bool TransactionController::addTransaction(
    qulonglong cellId,
    const QString& type,
    const QString& description,
    const QString& amount,
    const QString& occurredAt,
    qulonglong categoryId)
{
    clearError();
    const auto parsedType = parseType(type);
    const auto parsedAmount = parseAmount(amount);
    if (!parsedType || !parsedAmount || !TransactionService::isDateValid(occurredAt.toStdString()))
    {
        setErrorMessage(QStringLiteral(
            "Enter a positive amount with up to two decimals and select a valid date."));
        return false;
    }
    try
    {
        const auto created = m_transactionService.addTransaction(
            m_session.userId(), cellId, *parsedType, description.toStdString(),
            *parsedAmount, occurredAt.toStdString(), categoryId);
        if (!created)
        {
            setErrorMessage(QStringLiteral(
                "The transaction is invalid or is not allowed for this user."));
            return false;
        }
        return loadTransactions(cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Transaction creation failed:" << error.what();
        setErrorMessage(QStringLiteral("The transaction could not be saved."));
        return false;
    }
}

bool TransactionController::updateTransaction(
    qulonglong cellId,
    const QString& type,
    const QString& description,
    const QString& amount,
    const QString& occurredAt,
    qulonglong categoryId)
{
    clearError();
    const auto parsedType = parseType(type);
    const auto parsedAmount = parseAmount(amount);
    const qulonglong transactionId = m_selectedTransaction
        .value(QStringLiteral("transactionId")).toULongLong();
    if (transactionId == 0 || !parsedType || !parsedAmount ||
        !TransactionService::isDateValid(occurredAt.toStdString()))
    {
        setErrorMessage(QStringLiteral(
            "Enter a positive amount with up to two decimals and select a valid date."));
        return false;
    }
    try
    {
        if (!m_transactionService.editTransaction(
                m_session.userId(), cellId, transactionId, *parsedType,
                description.toStdString(), *parsedAmount,
                occurredAt.toStdString(), categoryId))
        {
            setErrorMessage(QStringLiteral(
                "The transaction is invalid or cannot be edited by this user."));
            return false;
        }
        clearSelection();
        return loadTransactions(cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Transaction update failed:" << error.what();
        setErrorMessage(QStringLiteral("The transaction changes could not be saved."));
        return false;
    }
}

bool TransactionController::deleteTransaction(
    qulonglong cellId,
    qulonglong transactionId)
{
    clearError();
    try
    {
        if (!m_transactionService.deleteTransaction(
                m_session.userId(), cellId, transactionId))
        {
            setErrorMessage(QStringLiteral("This transaction cannot be deleted."));
            return false;
        }
        clearSelection();
        return loadTransactions(cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Transaction deletion failed:" << error.what();
        setErrorMessage(QStringLiteral("The transaction could not be deleted."));
        return false;
    }
}

void TransactionController::clearError() { setErrorMessage({}); }

void TransactionController::clearTransactions()
{
    if (m_transactions.isEmpty() && !m_canWrite && m_balanceText.isEmpty()) return;
    m_transactions.clear();
    m_canWrite = false;
    m_balanceText.clear();
    emit transactionsChanged();
}

void TransactionController::setErrorMessage(const QString& message)
{
    if (m_errorMessage == message) return;
    m_errorMessage = message;
    emit errorMessageChanged();
}
