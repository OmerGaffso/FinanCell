#include "ui/qt/controllers/CellController.h"

#include <QDebug>
#include <QVariantMap>

#include <cstdint>
#include <exception>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "application/CellService.h"
#include "application/TransactionService.h"
#include "ui/qt/session/SessionState.h"

namespace
{
QString formatMoney(std::int64_t amountInMinorUnits, const std::string& currency)
{
    const bool negative = amountInMinorUnits < 0;
    const std::uint64_t magnitude = negative
        ? static_cast<std::uint64_t>(-(amountInMinorUnits + 1)) + 1
        : static_cast<std::uint64_t>(amountInMinorUnits);
    std::ostringstream output;
    if (negative) output << '-';
    output << magnitude / 100 << '.' << std::setw(2) << std::setfill('0')
           << magnitude % 100 << ' ' << currency;
    return QString::fromStdString(output.str());
}

QVariantMap toVariantMap(
    const FinancialCell& domainCell,
    std::int64_t balanceInMinorUnits)
{
    QVariantMap cell;
    cell.insert(
        QStringLiteral("cellId"),
        QVariant::fromValue<qulonglong>(domainCell.getCellId()));
    cell.insert(
        QStringLiteral("ownerId"),
        QVariant::fromValue<qulonglong>(domainCell.getOwnerId()));
    cell.insert(
        QStringLiteral("name"),
        QString::fromStdString(domainCell.getCellName()));
    cell.insert(
        QStringLiteral("description"),
        QString::fromStdString(domainCell.getCellDescription()));
    cell.insert(
        QStringLiteral("currency"),
        QString::fromStdString(domainCell.getCurrency()));
    cell.insert(
        QStringLiteral("balanceInMinorUnits"),
        QVariant::fromValue<qlonglong>(balanceInMinorUnits));
    cell.insert(
        QStringLiteral("balanceText"),
        formatMoney(balanceInMinorUnits, domainCell.getCurrency()));
    return cell;
}
}

CellController::CellController(
    CellService& cellService,
    TransactionService& transactionService,
    SessionState& session,
    QObject* parent)
    : QObject(parent),
      m_cellService(cellService),
      m_transactionService(transactionService),
      m_session(session)
{
    connect(
        &m_session,
        &SessionState::sessionChanged,
        this,
        [this]()
        {
            clearCells();
            clearSelection();
            clearError();
        });
}

QVariantList CellController::cells() const
{
    return m_cells;
}

QString CellController::errorMessage() const
{
    return m_errorMessage;
}

QVariantMap CellController::selectedCell() const
{
    return m_selectedCell;
}

bool CellController::hasSelectedCell() const
{
    return !m_selectedCell.isEmpty();
}

bool CellController::loadCells()
{
    clearError();
    if (!m_session.loggedIn())
    {
        clearCells();
        setErrorMessage(QStringLiteral("Sign in to view financial cells."));
        return false;
    }

    try
    {
        const auto domainCells = m_cellService.getCellsForUser(m_session.userId());
        QVariantList cells;
        cells.reserve(static_cast<qsizetype>(domainCells.size()));
        for (const auto& domainCell : domainCells)
        {
            const auto balance = m_transactionService.getCellBalance(
                m_session.userId(), domainCell.getCellId());
            if (!balance)
            {
                throw std::runtime_error(
                    "An accessible financial cell did not expose its balance.");
            }
            cells.append(toVariantMap(domainCell, *balance));
        }

        m_cells = std::move(cells);
        emit cellsChanged();
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Financial-cell loading failed:" << error.what();
        clearCells();
        setErrorMessage(QStringLiteral(
            "Financial cells could not be loaded because the database operation failed."));
        return false;
    }
}

bool CellController::createCell(
    const QString& name,
    const QString& description)
{
    clearError();
    if (!m_session.loggedIn())
    {
        setErrorMessage(QStringLiteral("Sign in to create a financial cell."));
        return false;
    }

    const std::string serviceName = name.toStdString();
    const std::string serviceDescription = description.toStdString();
    if (!m_cellService.isCellNameValid(serviceName))
    {
        setErrorMessage(QStringLiteral(
            "Cell names must be between 3 and 50 characters."));
        return false;
    }
    if (!m_cellService.isDescriptionValid(serviceDescription))
    {
        setErrorMessage(QStringLiteral(
            "Cell descriptions cannot exceed 200 characters."));
        return false;
    }

    try
    {
        if (!m_cellService.createCell(
                serviceName, m_session.userId(), serviceDescription))
        {
            setErrorMessage(QStringLiteral("The financial cell could not be created."));
            return false;
        }

        loadCells();
        return true;
    }
    catch (const std::invalid_argument& error)
    {
        setErrorMessage(QString::fromStdString(error.what()));
        return false;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Financial-cell creation failed:" << error.what();
        setErrorMessage(QStringLiteral(
            "The financial cell could not be created because the database operation failed."));
        return false;
    }
}

bool CellController::selectCell(qulonglong cellId)
{
    clearError();
    if (!m_session.loggedIn() || cellId == 0)
    {
        clearSelection();
        setErrorMessage(QStringLiteral("This financial cell cannot be opened."));
        return false;
    }

    try
    {
        const auto cell = m_cellService.getCellForUser(
            m_session.userId(), static_cast<std::uint64_t>(cellId));
        if (!cell)
        {
            clearSelection();
            setErrorMessage(QStringLiteral(
                "The financial cell does not exist or is not available to this user."));
            return false;
        }

        const auto balance = m_transactionService.getCellBalance(
            m_session.userId(), cell->getCellId());
        if (!balance)
        {
            clearSelection();
            setErrorMessage(QStringLiteral(
                "The current balance is not available for this financial cell."));
            return false;
        }

        m_selectedCell = toVariantMap(*cell, *balance);
        emit selectedCellChanged();
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Financial-cell selection failed:" << error.what();
        clearSelection();
        setErrorMessage(QStringLiteral(
            "The financial cell could not be opened because the database operation failed."));
        return false;
    }
}

void CellController::clearSelection()
{
    if (m_selectedCell.isEmpty()) return;
    m_selectedCell.clear();
    emit selectedCellChanged();
}

void CellController::clearError()
{
    setErrorMessage(QString());
}

void CellController::clearCells()
{
    if (m_cells.isEmpty()) return;
    m_cells.clear();
    emit cellsChanged();
}

void CellController::setErrorMessage(const QString& message)
{
    if (m_errorMessage == message) return;
    m_errorMessage = message;
    emit errorMessageChanged();
}
