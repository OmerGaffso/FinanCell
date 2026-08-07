#include "ui/qt/controllers/MemberController.h"

#include <QDebug>
#include <QVariantMap>

#include <cstdint>
#include <exception>
#include <optional>
#include <utility>

#include "application/CellService.h"
#include "ui/qt/session/SessionState.h"

namespace
{
QString roleText(CellRole role)
{
    if (role == CellRole::MANAGER) return QStringLiteral("MANAGER");
    if (role == CellRole::MEMBER) return QStringLiteral("MEMBER");
    return QStringLiteral("GUEST");
}

std::optional<CellRole> parseManagedRole(const QString& role)
{
    const QString normalized = role.trimmed().toUpper();
    if (normalized == QStringLiteral("MANAGER")) return CellRole::MANAGER;
    if (normalized == QStringLiteral("MEMBER")) return CellRole::MEMBER;
    if (normalized == QStringLiteral("GUEST")) return CellRole::GUEST;
    return std::nullopt;
}

QString resultMessage(CellOperationResult result)
{
    switch (result)
    {
        case CellOperationResult::SUCCESS: return {};
        case CellOperationResult::CELL_NOT_FOUND:
            return QStringLiteral("The financial cell no longer exists.");
        case CellOperationResult::USER_NOT_FOUND:
            return QStringLiteral("The selected user no longer exists.");
        case CellOperationResult::ALREADY_MEMBER:
            return QStringLiteral("That user is already a member of this cell.");
        case CellOperationResult::MEMBER_NOT_FOUND:
            return QStringLiteral("That user is no longer a member of this cell.");
        case CellOperationResult::INVALID_ROLE:
            return QStringLiteral("Choose Manager, Member, or Guest.");
        case CellOperationResult::NOT_AUTHORIZED:
            return QStringLiteral("Your role does not allow that membership change.");
        case CellOperationResult::LAST_MANAGER_REQUIRED:
            return QStringLiteral("A financial cell must keep at least one manager.");
        case CellOperationResult::INVALID_INPUT:
            return QStringLiteral("The membership request is invalid.");
        case CellOperationResult::STORAGE_ERROR:
            return QStringLiteral("The membership change could not be saved.");
    }
    return QStringLiteral("The membership change could not be completed.");
}
}

MemberController::MemberController(
    CellService& cellService,
    SessionState& session,
    QObject* parent)
    : QObject(parent), m_cellService(cellService), m_session(session)
{
    connect(&m_session, &SessionState::sessionChanged, this, [this]()
    {
        clearMembers();
        clearError();
    });
}

QVariantList MemberController::members() const { return m_members; }
bool MemberController::canManage() const { return m_canManage; }
bool MemberController::canAddMembers() const { return m_canAddMembers; }
QString MemberController::errorMessage() const { return m_errorMessage; }

bool MemberController::loadMembers(qulonglong cellId)
{
    clearError();
    if (!m_session.loggedIn() || cellId == 0)
    {
        clearMembers();
        setErrorMessage(QStringLiteral("Open a financial cell to view its members."));
        return false;
    }

    try
    {
        const auto summaries = m_cellService.getCellMemberSummaries(
            m_session.userId(), static_cast<std::uint64_t>(cellId));
        if (!summaries)
        {
            clearMembers();
            setErrorMessage(QStringLiteral("This cell is not available to the active user."));
            return false;
        }

        QVariantList members;
        bool canManage = false;
        bool canAddMembers = false;
        for (const auto& summary : *summaries)
        {
            QVariantMap member;
            member.insert(QStringLiteral("userId"),
                          QVariant::fromValue<qulonglong>(summary.getUserId()));
            member.insert(QStringLiteral("username"),
                          QString::fromStdString(summary.getUsername()));
            member.insert(QStringLiteral("displayName"),
                          QString::fromStdString(summary.getDisplayName()));
            member.insert(QStringLiteral("role"), roleText(summary.getRole()));
            member.insert(QStringLiteral("isManager"),
                          summary.getRole() == CellRole::MANAGER);
            if (summary.getUserId() == m_session.userId())
            {
                canManage = summary.getRole() == CellRole::MANAGER;
                canAddMembers = summary.getRole() != CellRole::GUEST;
            }
            members.append(member);
        }
        m_members = std::move(members);
        m_canManage = canManage;
        m_canAddMembers = canAddMembers;
        emit membersChanged();
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Cell-member loading failed:" << error.what();
        clearMembers();
        setErrorMessage(QStringLiteral(
            "Members could not be loaded because the database operation failed."));
        return false;
    }
}

bool MemberController::addMember(
    qulonglong cellId,
    qulonglong userId,
    const QString& role)
{
    const auto parsedRole = parseManagedRole(role);
    if (!parsedRole)
    {
        setErrorMessage(QStringLiteral("Choose Manager, Member, or Guest."));
        return false;
    }
    try
    {
        return finishMutation(static_cast<int>(m_cellService.addMemberToCell(
            m_session.userId(), cellId, userId, *parsedRole)), cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Adding a cell member failed:" << error.what();
        setErrorMessage(QStringLiteral("The new member could not be saved."));
        return false;
    }
}

bool MemberController::updateMemberRole(
    qulonglong cellId,
    qulonglong userId,
    const QString& role)
{
    const auto parsedRole = parseManagedRole(role);
    if (!parsedRole)
    {
        setErrorMessage(QStringLiteral("Choose Manager, Member, or Guest."));
        return false;
    }
    try
    {
        return finishMutation(static_cast<int>(m_cellService.updateMemberRole(
            m_session.userId(), cellId, userId, *parsedRole)), cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Changing a member role failed:" << error.what();
        setErrorMessage(QStringLiteral("The member role could not be saved."));
        return false;
    }
}

bool MemberController::removeMember(qulonglong cellId, qulonglong userId)
{
    try
    {
        return finishMutation(static_cast<int>(m_cellService.removeMemberFromCell(
            m_session.userId(), cellId, userId)), cellId);
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "Removing a cell member failed:" << error.what();
        setErrorMessage(QStringLiteral("The member could not be removed."));
        return false;
    }
}

void MemberController::clearError() { setErrorMessage({}); }

bool MemberController::finishMutation(int resultValue, qulonglong cellId)
{
    clearError();
    const auto result = static_cast<CellOperationResult>(resultValue);
    if (result != CellOperationResult::SUCCESS)
    {
        setErrorMessage(resultMessage(result));
        return false;
    }
    return loadMembers(cellId);
}

void MemberController::clearMembers()
{
    if (m_members.isEmpty() && !m_canManage && !m_canAddMembers) return;
    m_members.clear();
    m_canManage = false;
    m_canAddMembers = false;
    emit membersChanged();
}

void MemberController::setErrorMessage(const QString& message)
{
    if (m_errorMessage == message) return;
    m_errorMessage = message;
    emit errorMessageChanged();
}
