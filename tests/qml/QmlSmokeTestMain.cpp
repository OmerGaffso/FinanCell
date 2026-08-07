#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QtQuickTest/quicktest.h>

namespace
{
/** @brief Records QML runtime failures that Qt Quick Test otherwise logs as warnings. */
class QmlDiagnostics final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList errors READ errors)

public:
    /** @brief Installs the diagnostic message handler. */
    explicit QmlDiagnostics(QObject* parent = nullptr)
        : QObject(parent), m_previousHandler(qInstallMessageHandler(handleMessage))
    {
        s_instance = this;
    }

    /** @brief Restores the process message handler. */
    ~QmlDiagnostics() override
    {
        s_instance = nullptr;
        qInstallMessageHandler(m_previousHandler);
    }

    /** @brief Returns relevant QML runtime failures observed since the last clear. */
    QStringList errors() const
    {
        QMutexLocker lock(&m_mutex);
        return m_errors;
    }

    /** @brief Clears captured QML runtime failures before a test flow begins. */
    Q_INVOKABLE void clear()
    {
        QMutexLocker lock(&m_mutex);
        m_errors.clear();
    }

private:
    static void handleMessage(
        QtMsgType type, const QMessageLogContext& context, const QString& message)
    {
        if (s_instance && (type == QtWarningMsg || type == QtCriticalMsg) &&
            (message.contains(QStringLiteral("Binding loop detected")) ||
             message.contains(QStringLiteral("TypeError:")) ||
             message.contains(QStringLiteral("ReferenceError:")) ||
             message.contains(QStringLiteral("Required property")) ||
             message.contains(QStringLiteral("Cannot read property")) ||
             message.contains(QStringLiteral("Cannot call method")) ||
             message.contains(QStringLiteral("Unable to assign"))))
        {
            QMutexLocker lock(&s_instance->m_mutex);
            s_instance->m_errors.append(message);
        }
        if (s_instance && s_instance->m_previousHandler)
            s_instance->m_previousHandler(type, context, message);
    }

    inline static QmlDiagnostics* s_instance{nullptr};
    mutable QMutex m_mutex;
    QStringList m_errors;
    QtMessageHandler m_previousHandler;
};

/** @brief Superset mock used to satisfy every Qt-facing controller dependency in QML. */
class MockController final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ loggedIn WRITE setLoggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString username READ username NOTIFY stateChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY stateChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY stateChanged)
    Q_PROPERTY(QVariantList users READ users NOTIFY stateChanged)
    Q_PROPERTY(QVariantList cells READ cells NOTIFY stateChanged)
    Q_PROPERTY(QVariantMap selectedCell READ selectedCell NOTIFY stateChanged)
    Q_PROPERTY(bool hasSelectedCell READ trueValue NOTIFY stateChanged)
    Q_PROPERTY(bool canManageSelectedCell READ trueValue NOTIFY stateChanged)
    Q_PROPERTY(QVariantList members READ members NOTIFY stateChanged)
    Q_PROPERTY(bool canManage READ trueValue NOTIFY stateChanged)
    Q_PROPERTY(bool canAddMembers READ trueValue NOTIFY stateChanged)
    Q_PROPERTY(QVariantList categories READ categories NOTIFY stateChanged)
    Q_PROPERTY(bool canCreate READ trueValue NOTIFY stateChanged)
    Q_PROPERTY(QVariantList transactions READ transactions NOTIFY stateChanged)
    Q_PROPERTY(QVariantMap selectedTransaction READ selectedTransaction NOTIFY stateChanged)
    Q_PROPERTY(bool hasSelectedTransaction READ hasSelectedTransaction WRITE setHasSelectedTransaction NOTIFY stateChanged)
    Q_PROPERTY(bool canWrite READ trueValue NOTIFY stateChanged)
    Q_PROPERTY(QString balanceText READ balanceText NOTIFY stateChanged)
    Q_PROPERTY(bool hasReport READ trueValue NOTIFY stateChanged)
    Q_PROPERTY(QString totalIncomeText READ balanceText NOTIFY stateChanged)
    Q_PROPERTY(QString totalExpensesText READ balanceText NOTIFY stateChanged)
    Q_PROPERTY(QVariantList categoryLines READ categoryLines NOTIFY stateChanged)

public:
    /** @brief Creates deterministic controller state for navigation tests. */
    explicit MockController(QObject* parent = nullptr) : QObject(parent)
    {
        m_selectedCell.insert(QStringLiteral("cellId"), 42);
        m_selectedCell.insert(QStringLiteral("name"), QStringLiteral("Test cell"));
        m_selectedCell.insert(QStringLiteral("description"), QStringLiteral("Test data"));
        m_selectedCell.insert(QStringLiteral("currency"), QStringLiteral("ILS"));
        m_selectedCell.insert(QStringLiteral("balanceText"), QStringLiteral("80.00 ILS"));
        m_selectedCell.insert(QStringLiteral("balanceInMinorUnits"), 8000);

        for (int index = 1; index <= 8; ++index)
        {
            QVariantMap cell;
            cell.insert(QStringLiteral("cellId"), index);
            cell.insert(QStringLiteral("name"), QStringLiteral("Cell %1").arg(index));
            cell.insert(QStringLiteral("balanceText"), QStringLiteral("80.00 ILS"));
            cell.insert(QStringLiteral("balanceInMinorUnits"), 8000);
            m_cells.append(cell);
        }

        QVariantMap user;
        user.insert(QStringLiteral("userId"), 2);
        user.insert(QStringLiteral("username"), QStringLiteral("member"));
        user.insert(QStringLiteral("displayName"), QStringLiteral("Cell Member"));
        m_users.append(user);

        QVariantMap owner;
        owner.insert(QStringLiteral("userId"), 1);
        owner.insert(QStringLiteral("username"), QStringLiteral("tester"));
        owner.insert(QStringLiteral("displayName"), QStringLiteral("Test User"));
        owner.insert(QStringLiteral("role"), QStringLiteral("MANAGER"));
        owner.insert(QStringLiteral("isManager"), true);
        m_members.append(owner);

        QVariantMap member = user;
        member.insert(QStringLiteral("role"), QStringLiteral("MEMBER"));
        member.insert(QStringLiteral("isManager"), false);
        m_members.append(member);

        QVariantMap category;
        category.insert(QStringLiteral("categoryId"), 7);
        category.insert(QStringLiteral("name"), QStringLiteral("General"));
        category.insert(QStringLiteral("budgetText"), QStringLiteral("500.00 ILS"));
        category.insert(QStringLiteral("budgetInput"), QStringLiteral("500.00"));
        category.insert(QStringLiteral("hasBudget"), true);
        m_categories.append(category);

        QVariantMap transaction;
        transaction.insert(QStringLiteral("transactionId"), 9);
        transaction.insert(QStringLiteral("description"), QStringLiteral("Groceries"));
        transaction.insert(QStringLiteral("amountText"), QStringLiteral("20.00 ILS"));
        transaction.insert(QStringLiteral("type"), QStringLiteral("EXPENSE"));
        transaction.insert(QStringLiteral("categoryName"), QStringLiteral("General"));
        transaction.insert(QStringLiteral("dateText"), QStringLiteral("05/08/26"));
        transaction.insert(QStringLiteral("editable"), true);
        m_transactions.append(transaction);

        QVariantMap reportLine;
        reportLine.insert(QStringLiteral("categoryName"), QStringLiteral("General"));
        reportLine.insert(QStringLiteral("incomeText"), QStringLiteral("100.00 ILS"));
        reportLine.insert(QStringLiteral("expensesText"), QStringLiteral("20.00 ILS"));
        reportLine.insert(QStringLiteral("hasBudget"), true);
        reportLine.insert(QStringLiteral("budgetText"), QStringLiteral("500.00 ILS"));
        reportLine.insert(QStringLiteral("budgetProgress"), 0.04);
        reportLine.insert(QStringLiteral("budgetUsageText"), QStringLiteral("4.0% used"));
        reportLine.insert(QStringLiteral("budgetStatusText"), QStringLiteral("480.00 ILS remaining"));
        reportLine.insert(QStringLiteral("overBudget"), false);
        m_categoryLines.append(reportLine);

        m_selectedTransaction.insert(QStringLiteral("type"), QStringLiteral("INCOME"));
        m_selectedTransaction.insert(QStringLiteral("description"), QStringLiteral("Salary"));
        m_selectedTransaction.insert(QStringLiteral("amountInput"), QStringLiteral("10.00"));
        m_selectedTransaction.insert(QStringLiteral("dateInput"), QStringLiteral("2026-08-05"));
        m_selectedTransaction.insert(QStringLiteral("categoryId"), 7);
    }

    bool loggedIn() const { return m_loggedIn; }
    QString username() const { return QStringLiteral("tester"); }
    QString displayName() const { return QStringLiteral("Test User"); }
    QString errorMessage() const { return {}; }
    QVariantList emptyList() const { return {}; }
    QVariantList users() const { return m_users; }
    QVariantList cells() const { return m_cells; }
    QVariantList members() const { return m_members; }
    QVariantList categories() const { return m_categories; }
    QVariantList transactions() const { return m_transactions; }
    QVariantList categoryLines() const { return m_categoryLines; }
    QVariantMap selectedCell() const { return m_selectedCell; }
    QVariantMap selectedTransaction() const { return m_selectedTransaction; }
    bool hasSelectedTransaction() const { return m_hasSelectedTransaction; }
    bool trueValue() const { return true; }
    bool falseValue() const { return false; }
    QString balanceText() const { return QStringLiteral("0.00 ILS"); }

    /** @brief Changes mock authentication state and triggers the real Main.qml route. */
    Q_INVOKABLE void setLoggedIn(bool loggedIn)
    {
        if (m_loggedIn == loggedIn) return;
        m_loggedIn = loggedIn;
        emit loggedInChanged();
    }

    /** @brief Selects whether the transaction form initializes in edit mode. */
    Q_INVOKABLE void setHasSelectedTransaction(bool selected)
    {
        if (m_hasSelectedTransaction == selected) return;
        m_hasSelectedTransaction = selected;
        emit stateChanged();
    }

    Q_INVOKABLE bool registerUser(const QString&, const QString&, const QString&) { return true; }
    Q_INVOKABLE bool login(const QString&, const QString&) { setLoggedIn(true); return true; }
    Q_INVOKABLE bool searchUsers(const QString&) { return true; }
    Q_INVOKABLE void logout() { setLoggedIn(false); }
    Q_INVOKABLE void clearError() {}
    Q_INVOKABLE bool loadCells() { return true; }
    Q_INVOKABLE bool createCell(const QString&, const QString&) { return true; }
    Q_INVOKABLE bool selectCell(qulonglong) { return true; }
    Q_INVOKABLE void clearSelection() {}
    Q_INVOKABLE bool updateSelectedCell(const QString&, const QString&) { return true; }
    Q_INVOKABLE bool deleteSelectedCell() { return true; }
    Q_INVOKABLE bool loadMembers(qulonglong) { return true; }
    Q_INVOKABLE bool addMember(qulonglong, qulonglong, const QString&) { return true; }
    Q_INVOKABLE bool updateMemberRole(qulonglong, qulonglong, const QString&) { return true; }
    Q_INVOKABLE bool removeMember(qulonglong, qulonglong) { return true; }
    Q_INVOKABLE bool loadCategories(qulonglong) { return true; }
    Q_INVOKABLE bool createCategory(qulonglong, const QString&) { return true; }
    Q_INVOKABLE bool setMonthlyBudget(qulonglong, qulonglong, const QString&) { return true; }
    Q_INVOKABLE bool clearMonthlyBudget(qulonglong, qulonglong) { return true; }
    Q_INVOKABLE bool loadTransactions(qulonglong) { return true; }
    Q_INVOKABLE bool loadTransactions(qulonglong, const QString&, const QString&) { return true; }
    Q_INVOKABLE bool selectTransaction(qulonglong) { return true; }
    Q_INVOKABLE bool addTransaction(
        qulonglong, const QString&, const QString&, const QString&, const QString&, qulonglong)
    {
        return true;
    }
    Q_INVOKABLE bool updateTransaction(
        qulonglong, const QString&, const QString&, const QString&, const QString&, qulonglong)
    {
        return true;
    }
    Q_INVOKABLE bool deleteTransaction(qulonglong, qulonglong) { return true; }
    Q_INVOKABLE bool generateReport(qulonglong, const QString&) { return true; }
    Q_INVOKABLE void clearReport() {}

signals:
    /** @brief Mirrors the authentication signal consumed by Main.qml. */
    void loggedInChanged();
    /** @brief Notifies QML when mutable mock form state changes. */
    void stateChanged();

private:
    bool m_loggedIn{false};
    bool m_hasSelectedTransaction{false};
    QVariantList m_users;
    QVariantList m_cells;
    QVariantList m_members;
    QVariantList m_categories;
    QVariantList m_transactions;
    QVariantList m_categoryLines;
    QVariantMap m_selectedCell;
    QVariantMap m_selectedTransaction;
};

/** @brief Supplies mock controllers and diagnostics to the real QML application root. */
class QmlSmokeTestSetup final : public QObject
{
    Q_OBJECT

public slots:
    /** @brief Installs test-only context properties before Main.qml is created. */
    void qmlEngineAvailable(QQmlEngine* engine)
    {
        QQmlContext* context = engine->rootContext();
        context->setContextProperty(QStringLiteral("startupError"), QString{});
        context->setContextProperty(QStringLiteral("userController"), &m_user);
        context->setContextProperty(QStringLiteral("cellController"), &m_cell);
        context->setContextProperty(QStringLiteral("memberController"), &m_member);
        context->setContextProperty(QStringLiteral("categoryController"), &m_category);
        context->setContextProperty(QStringLiteral("transactionController"), &m_transaction);
        context->setContextProperty(QStringLiteral("reportController"), &m_report);
        context->setContextProperty(QStringLiteral("testDiagnostics"), &m_diagnostics);
    }

private:
    QmlDiagnostics m_diagnostics;
    MockController m_user;
    MockController m_cell;
    MockController m_member;
    MockController m_category;
    MockController m_transaction;
    MockController m_report;
};
}

QUICK_TEST_MAIN_WITH_SETUP(financell_qml_smoke, QmlSmokeTestSetup)

#include "QmlSmokeTestMain.moc"
