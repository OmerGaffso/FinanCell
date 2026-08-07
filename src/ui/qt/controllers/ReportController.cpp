#include "ui/qt/controllers/ReportController.h"

#include <QDebug>
#include <QVariantMap>

#include <cstdint>
#include <exception>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <string>

#include "application/CellService.h"
#include "application/MonthlyReportService.h"
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

QString formatPercentage(long double ratio)
{
    std::ostringstream output;
    output << std::fixed << std::setprecision(1)
           << static_cast<double>(ratio * 100.0L) << "% used";
    return QString::fromStdString(output.str());
}
}

ReportController::ReportController(
    MonthlyReportService& reportService,
    CellService& cellService,
    SessionState& session,
    QObject* parent)
    : QObject(parent),
      m_reportService(reportService),
      m_cellService(cellService),
      m_session(session)
{
    connect(&m_session, &SessionState::sessionChanged, this, [this]()
    {
        clearReport();
        clearError();
    });
}

bool ReportController::hasReport() const { return m_hasReport; }
QString ReportController::month() const { return m_month; }
QString ReportController::totalIncomeText() const { return m_totalIncomeText; }
QString ReportController::totalExpensesText() const { return m_totalExpensesText; }
QString ReportController::balanceText() const { return m_balanceText; }
QVariantList ReportController::categoryLines() const { return m_categoryLines; }
QString ReportController::errorMessage() const { return m_errorMessage; }

bool ReportController::generateReport(qulonglong cellId, const QString& month)
{
    clearError();
    try
    {
        const auto cell = m_cellService.getCellForUser(m_session.userId(), cellId);
        const auto report = m_reportService.generate(
            m_session.userId(), cellId, month.toStdString());
        if (!cell || !report)
        {
            clearReport();
            setErrorMessage(QStringLiteral(
                "Select a valid month for an accessible cell."));
            return false;
        }

        QVariantList lines;
        for (const auto& line : report->categories)
        {
            QVariantMap value;
            value.insert(QStringLiteral("categoryId"),
                         QVariant::fromValue<qulonglong>(line.categoryId));
            value.insert(QStringLiteral("categoryName"),
                         QString::fromStdString(line.categoryName));
            value.insert(QStringLiteral("incomeText"),
                         formatMoney(line.incomeInMinorUnits, cell->getCurrency()));
            value.insert(QStringLiteral("expensesText"),
                         formatMoney(line.expensesInMinorUnits, cell->getCurrency()));
            const bool hasBudget = line.monthlyBudgetInMinorUnits > 0;
            value.insert(QStringLiteral("hasBudget"), hasBudget);
            value.insert(QStringLiteral("overBudget"), line.overBudget);
            if (hasBudget)
            {
                const long double usage =
                    static_cast<long double>(line.expensesInMinorUnits) /
                    static_cast<long double>(line.monthlyBudgetInMinorUnits);
                value.insert(QStringLiteral("budgetText"),
                             formatMoney(line.monthlyBudgetInMinorUnits,
                                         cell->getCurrency()));
                value.insert(QStringLiteral("remainingBudgetText"),
                             formatMoney(line.remainingBudgetInMinorUnits,
                                         cell->getCurrency()));
                value.insert(QStringLiteral("budgetUsageText"),
                             formatPercentage(usage));
                value.insert(QStringLiteral("budgetProgress"),
                             static_cast<double>(std::min(usage, 1.0L)));
                value.insert(
                    QStringLiteral("budgetStatusText"),
                    line.overBudget
                        ? QStringLiteral("%1 over budget").arg(formatMoney(
                              -line.remainingBudgetInMinorUnits,
                              cell->getCurrency()))
                        : QStringLiteral("%1 remaining").arg(formatMoney(
                              line.remainingBudgetInMinorUnits,
                              cell->getCurrency())));
            }
            lines.append(value);
        }
        m_hasReport = true;
        m_month = QString::fromStdString(report->month);
        m_totalIncomeText = formatMoney(
            report->totalIncomeInMinorUnits, cell->getCurrency());
        m_totalExpensesText = formatMoney(
            report->totalExpensesInMinorUnits, cell->getCurrency());
        m_balanceText = formatMoney(report->balanceInMinorUnits, cell->getCurrency());
        m_categoryLines = lines;
        emit reportChanged();
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Monthly report generation failed:" << error.what();
        clearReport();
        setErrorMessage(QStringLiteral("The monthly report could not be generated."));
        return false;
    }
}

void ReportController::clearReport()
{
    if (!m_hasReport && m_month.isEmpty() && m_categoryLines.isEmpty()) return;
    m_hasReport = false;
    m_month.clear();
    m_totalIncomeText.clear();
    m_totalExpensesText.clear();
    m_balanceText.clear();
    m_categoryLines.clear();
    emit reportChanged();
}

void ReportController::clearError() { setErrorMessage({}); }

void ReportController::setErrorMessage(const QString& message)
{
    if (m_errorMessage == message) return;
    m_errorMessage = message;
    emit errorMessageChanged();
}
