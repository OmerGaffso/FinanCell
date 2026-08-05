#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class CellService;
class SessionState;
class TransactionService;

/** @brief Qt-facing adapter for financial cells visible to the active user. */
class CellController final : public QObject
{
    Q_OBJECT
    /** @brief Financial cells returned by the latest load. */
    Q_PROPERTY(QVariantList cells READ cells NOTIFY cellsChanged)
    /** @brief Latest user-facing cell-operation error. */
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    /** @brief Financial cell selected for the current GUI workflow. */
    Q_PROPERTY(QVariantMap selectedCell READ selectedCell NOTIFY selectedCellChanged)
    /** @brief Whether an accessible financial cell is selected. */
    Q_PROPERTY(bool hasSelectedCell READ hasSelectedCell NOTIFY selectedCellChanged)
    /** @brief Whether the active user owns the selected financial cell. */
    Q_PROPERTY(bool canManageSelectedCell READ canManageSelectedCell NOTIFY selectedCellChanged)

public:
    /** @brief Creates the controller. @param cellService Existing cell application service. @param transactionService Existing transaction application service. @param session Shared GUI session. @param parent Optional Qt owner. */
    CellController(
        CellService& cellService,
        TransactionService& transactionService,
        SessionState& session,
        QObject* parent = nullptr);

    /** @brief Returns cells visible to the active user. @return Cell summary maps for QML. */
    QVariantList cells() const;
    /** @brief Returns the latest user-facing failure. @return Error text, or empty. */
    QString errorMessage() const;
    /** @brief Returns the selected financial cell. @return Cell summary map, or empty. */
    QVariantMap selectedCell() const;
    /** @brief Returns whether a financial cell is selected. @return True when selected. */
    bool hasSelectedCell() const;
    /** @brief Returns whether the active user owns the selected cell. */
    bool canManageSelectedCell() const;

    /** @brief Loads cells available to the active user. @return True when loading completed. */
    Q_INVOKABLE bool loadCells();
    /** @brief Creates a financial cell for the active user. @param name Cell name. @param description Optional description. @return True when created. */
    Q_INVOKABLE bool createCell(
        const QString& name,
        const QString& description);
    /** @brief Selects an accessible financial cell. @param cellId Cell ID. @return True when selected. */
    Q_INVOKABLE bool selectCell(qulonglong cellId);
    /** @brief Clears the selected financial cell. */
    Q_INVOKABLE void clearSelection();
    /** @brief Updates the selected cell. @param name New name. @param description New description. @return True on success. */
    Q_INVOKABLE bool updateSelectedCell(
        const QString& name,
        const QString& description);
    /** @brief Permanently deletes the selected cell. @return True on success. */
    Q_INVOKABLE bool deleteSelectedCell();
    /** @brief Clears the current user-facing error. */
    Q_INVOKABLE void clearError();

signals:
    /** @brief Emitted when the visible cell list changes. */
    void cellsChanged();
    /** @brief Emitted when the user-facing error changes. */
    void errorMessageChanged();
    /** @brief Emitted when the selected financial cell changes. */
    void selectedCellChanged();

private:
    void clearCells();
    void setErrorMessage(const QString& message);

    CellService& m_cellService;
    TransactionService& m_transactionService;
    SessionState& m_session;
    QVariantList m_cells;
    QVariantMap m_selectedCell;
    QString m_errorMessage;
};
