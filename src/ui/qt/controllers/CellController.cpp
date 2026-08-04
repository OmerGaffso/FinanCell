#include "ui/qt/controllers/CellController.h"

#include <QDebug>
#include <QVariantMap>

#include <exception>
#include <utility>

#include "application/CellService.h"
#include "ui/qt/session/SessionState.h"

CellController::CellController(
    CellService& cellService,
    SessionState& session,
    QObject* parent)
    : QObject(parent), m_cellService(cellService), m_session(session)
{
    connect(
        &m_session,
        &SessionState::sessionChanged,
        this,
        [this]()
        {
            clearCells();
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
            cells.append(cell);
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
