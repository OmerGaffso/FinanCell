#include "ui/qt/session/SessionState.h"

SessionState::SessionState(QObject* parent)
    : QObject(parent)
{
}

bool SessionState::loggedIn() const
{
    return m_userId != 0;
}

std::uint64_t SessionState::userId() const
{
    return m_userId;
}

const QString& SessionState::username() const
{
    return m_username;
}

const QString& SessionState::displayName() const
{
    return m_displayName;
}

void SessionState::setUser(
    std::uint64_t userId,
    const QString& username,
    const QString& displayName)
{
    m_userId = userId;
    m_username = username;
    m_displayName = displayName;
    emit sessionChanged();
}

void SessionState::clear()
{
    if (!loggedIn() && m_username.isEmpty() && m_displayName.isEmpty()) return;

    m_userId = 0;
    m_username.clear();
    m_displayName.clear();
    emit sessionChanged();
}
