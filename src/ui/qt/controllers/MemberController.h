#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class CellService;
class SessionState;

/** @brief Qt-facing adapter for financial-cell membership management. */
class MemberController final : public QObject
{
    Q_OBJECT
    /** @brief Public members returned by the latest load. */
    Q_PROPERTY(QVariantList members READ members NOTIFY membersChanged)
    /** @brief Whether the active user can manage the loaded membership. */
    Q_PROPERTY(bool canManage READ canManage NOTIFY membersChanged)
    /** @brief Whether the active user can add members or guests. */
    Q_PROPERTY(bool canAddMembers READ canAddMembers NOTIFY membersChanged)
    /** @brief Latest user-facing membership error. */
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    /** @brief Creates the controller. @param cellService Existing cell service. @param session Shared GUI session. @param parent Optional Qt owner. */
    MemberController(
        CellService& cellService,
        SessionState& session,
        QObject* parent = nullptr);

    /** @brief Returns loaded member summaries. @return Member maps for QML. */
    QVariantList members() const;
    /** @brief Returns whether the active user is a manager. @return True for a manager. */
    bool canManage() const;
    /** @brief Returns whether the active user may add members or guests. */
    bool canAddMembers() const;
    /** @brief Returns the latest failure. @return Error text, or empty. */
    QString errorMessage() const;

    /** @brief Loads members of an accessible cell. @param cellId Cell ID. @return True when loaded. */
    Q_INVOKABLE bool loadMembers(qulonglong cellId);
    /** @brief Adds a registered user to a cell. @param cellId Cell ID. @param userId User ID. @param role MANAGER, MEMBER, or GUEST. @return True on success. */
    Q_INVOKABLE bool addMember(
        qulonglong cellId,
        qulonglong userId,
        const QString& role);
    /** @brief Changes a member role. @param cellId Cell ID. @param userId User ID. @param role MANAGER, MEMBER, or GUEST. @return True on success. */
    Q_INVOKABLE bool updateMemberRole(
        qulonglong cellId,
        qulonglong userId,
        const QString& role);
    /** @brief Removes a member. @param cellId Cell ID. @param userId User ID. @return True on success. */
    Q_INVOKABLE bool removeMember(qulonglong cellId, qulonglong userId);
    /** @brief Clears the latest error. */
    Q_INVOKABLE void clearError();

signals:
    /** @brief Emitted when member data or management access changes. */
    void membersChanged();
    /** @brief Emitted when the user-facing error changes. */
    void errorMessageChanged();

private:
    bool finishMutation(int result, qulonglong cellId);
    void clearMembers();
    void setErrorMessage(const QString& message);

    CellService& m_cellService;
    SessionState& m_session;
    QVariantList m_members;
    bool m_canManage{false};
    bool m_canAddMembers{false};
    QString m_errorMessage;
};
