#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class CellService;
class SessionState;
class TransactionService;

/** @brief Qt-facing adapter for cell transactions and exact money input. */
class TransactionController final : public QObject
{
    Q_OBJECT
    /** @brief Transactions returned by the latest load. */
    Q_PROPERTY(QVariantList transactions READ transactions NOTIFY transactionsChanged)
    /** @brief Transaction selected for editing. */
    Q_PROPERTY(QVariantMap selectedTransaction READ selectedTransaction NOTIFY selectedTransactionChanged)
    /** @brief Whether a transaction is selected. */
    Q_PROPERTY(bool hasSelectedTransaction READ hasSelectedTransaction NOTIFY selectedTransactionChanged)
    /** @brief Whether the active member may add transactions. */
    Q_PROPERTY(bool canWrite READ canWrite NOTIFY transactionsChanged)
    /** @brief Formatted current cell balance. */
    Q_PROPERTY(QString balanceText READ balanceText NOTIFY transactionsChanged)
    /** @brief Latest transaction-operation error. */
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    /** @brief Creates the controller. @param transactionService Transaction service. @param cellService Cell service used for role and currency presentation. @param session Shared GUI session. @param parent Optional Qt owner. */
    TransactionController(
        TransactionService& transactionService,
        CellService& cellService,
        SessionState& session,
        QObject* parent = nullptr);

    /** @brief Returns loaded transaction maps. */
    QVariantList transactions() const;
    /** @brief Returns the selected transaction map. */
    QVariantMap selectedTransaction() const;
    /** @brief Returns whether a transaction is selected. */
    bool hasSelectedTransaction() const;
    /** @brief Returns whether the active member can add transactions. */
    bool canWrite() const;
    /** @brief Returns the formatted current balance. */
    QString balanceText() const;
    /** @brief Returns the latest failure. */
    QString errorMessage() const;

    /** @brief Loads authorized transactions. @param cellId Cell ID. @param fromDate Optional YYYY-MM-DD start. @param toDate Optional YYYY-MM-DD end. @return True when loaded. */
    Q_INVOKABLE bool loadTransactions(
        qulonglong cellId,
        const QString& fromDate = {},
        const QString& toDate = {});
    /** @brief Selects a loaded transaction for editing. @param transactionId Transaction ID. @return True when selected. */
    Q_INVOKABLE bool selectTransaction(qulonglong transactionId);
    /** @brief Clears the selected transaction. */
    Q_INVOKABLE void clearSelection();
    /** @brief Adds income or an expense. @return True on success. */
    Q_INVOKABLE bool addTransaction(
        qulonglong cellId,
        const QString& type,
        const QString& description,
        const QString& amount,
        const QString& occurredAt,
        qulonglong categoryId);
    /** @brief Updates the selected transaction. @return True on success. */
    Q_INVOKABLE bool updateTransaction(
        qulonglong cellId,
        const QString& type,
        const QString& description,
        const QString& amount,
        const QString& occurredAt,
        qulonglong categoryId);
    /** @brief Deletes a transaction. @param cellId Cell ID. @param transactionId Transaction ID. @return True on success. */
    Q_INVOKABLE bool deleteTransaction(qulonglong cellId, qulonglong transactionId);
    /** @brief Clears the latest error. */
    Q_INVOKABLE void clearError();

signals:
    /** @brief Emitted when transaction data, balance, or write access changes. */
    void transactionsChanged();
    /** @brief Emitted when transaction selection changes. */
    void selectedTransactionChanged();
    /** @brief Emitted when the user-facing error changes. */
    void errorMessageChanged();

private:
    void clearTransactions();
    void setErrorMessage(const QString& message);

    TransactionService& m_transactionService;
    CellService& m_cellService;
    SessionState& m_session;
    QVariantList m_transactions;
    QVariantMap m_selectedTransaction;
    bool m_canWrite{false};
    QString m_balanceText;
    QString m_errorMessage;
};
