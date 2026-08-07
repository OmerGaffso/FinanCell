#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class CellService;
class MonthlyReportService;
class SessionState;

/** @brief Qt-facing adapter for authorized monthly financial reports. */
class ReportController final : public QObject
{
    Q_OBJECT
    /** @brief Whether a report has been generated. */
    Q_PROPERTY(bool hasReport READ hasReport NOTIFY reportChanged)
    /** @brief Generated report month. */
    Q_PROPERTY(QString month READ month NOTIFY reportChanged)
    /** @brief Formatted monthly income total. */
    Q_PROPERTY(QString totalIncomeText READ totalIncomeText NOTIFY reportChanged)
    /** @brief Formatted monthly expense total. */
    Q_PROPERTY(QString totalExpensesText READ totalExpensesText NOTIFY reportChanged)
    /** @brief Formatted monthly net balance. */
    Q_PROPERTY(QString balanceText READ balanceText NOTIFY reportChanged)
    /** @brief Per-category report lines. */
    Q_PROPERTY(QVariantList categoryLines READ categoryLines NOTIFY reportChanged)
    /** @brief Latest report-generation error. */
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    /** @brief Creates the controller. @param reportService Monthly report service. @param cellService Cell service used for currency presentation. @param session Shared GUI session. @param parent Optional Qt owner. */
    ReportController(
        MonthlyReportService& reportService,
        CellService& cellService,
        SessionState& session,
        QObject* parent = nullptr);

    /** @brief Returns whether a report is available. */
    bool hasReport() const;
    /** @brief Returns the report month. */
    QString month() const;
    /** @brief Returns formatted total income. */
    QString totalIncomeText() const;
    /** @brief Returns formatted total expenses. */
    QString totalExpensesText() const;
    /** @brief Returns the formatted monthly balance. */
    QString balanceText() const;
    /** @brief Returns category breakdown maps. */
    QVariantList categoryLines() const;
    /** @brief Returns the latest failure. */
    QString errorMessage() const;

    /** @brief Generates a report. @param cellId Cell ID. @param month YYYY-MM month. @return True on success. */
    Q_INVOKABLE bool generateReport(qulonglong cellId, const QString& month);
    /** @brief Clears report state. */
    Q_INVOKABLE void clearReport();
    /** @brief Clears the latest error. */
    Q_INVOKABLE void clearError();

signals:
    /** @brief Emitted when report content changes. */
    void reportChanged();
    /** @brief Emitted when the user-facing error changes. */
    void errorMessageChanged();

private:
    void setErrorMessage(const QString& message);

    MonthlyReportService& m_reportService;
    CellService& m_cellService;
    SessionState& m_session;
    bool m_hasReport{false};
    QString m_month;
    QString m_totalIncomeText;
    QString m_totalExpensesText;
    QString m_balanceText;
    QVariantList m_categoryLines;
    QString m_errorMessage;
};
