#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class CellService;
class SessionState;

/** @brief Qt-facing adapter for financial cells visible to the active user. */
class CellController final : public QObject
{
    Q_OBJECT
    /** @brief Financial cells returned by the latest load. */
    Q_PROPERTY(QVariantList cells READ cells NOTIFY cellsChanged)
    /** @brief Latest user-facing cell-operation error. */
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    /** @brief Creates the controller. @param cellService Existing cell application service. @param session Shared GUI session. @param parent Optional Qt owner. */
    CellController(
        CellService& cellService,
        SessionState& session,
        QObject* parent = nullptr);

    /** @brief Returns cells visible to the active user. @return Cell summary maps for QML. */
    QVariantList cells() const;
    /** @brief Returns the latest user-facing failure. @return Error text, or empty. */
    QString errorMessage() const;

    /** @brief Loads cells available to the active user. @return True when loading completed. */
    Q_INVOKABLE bool loadCells();
    /** @brief Clears the current user-facing error. */
    Q_INVOKABLE void clearError();

signals:
    /** @brief Emitted when the visible cell list changes. */
    void cellsChanged();
    /** @brief Emitted when the user-facing error changes. */
    void errorMessageChanged();

private:
    void clearCells();
    void setErrorMessage(const QString& message);

    CellService& m_cellService;
    SessionState& m_session;
    QVariantList m_cells;
    QString m_errorMessage;
};
