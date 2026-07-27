#include "ui/qt/controllers/UserController.h"

#include <QDebug>
#include <QVariantMap>

#include <exception>
#include <string>
#include <utility>

#include "application/UserService.h"

UserController::UserController(UserService& userService, QObject* parent)
    : QObject(parent), m_userService(userService)
{
}

bool UserController::loggedIn() const
{
    return m_loggedIn;
}

QString UserController::username() const
{
    return m_username;
}

QString UserController::displayName() const
{
    return m_displayName;
}

QString UserController::errorMessage() const
{
    return m_errorMessage;
}

QVariantList UserController::users() const
{
    return m_users;
}

bool UserController::registerUser(
    const QString& username,
    const QString& displayName,
    const QString& password)
{
    clearError();
    try
    {
        std::string serviceUsername = username.toStdString();
        std::string serviceDisplayName = displayName.toStdString();
        std::string servicePassword = password.toStdString();

        if (m_userService.userExists(serviceUsername))
        {
            setErrorMessage(QStringLiteral("That username is already registered."));
            return false;
        }

        if (!m_userService.createUser(
                serviceUsername, serviceDisplayName, servicePassword))
        {
            setErrorMessage(QStringLiteral(
                "Check the account details. Usernames must be 3–30 characters, "
                "display names 3–50 characters, and passwords 6–128 characters. "
                "Usernames and passwords cannot contain whitespace."));
            return false;
        }
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "User registration failed:" << error.what();
        setErrorMessage(QStringLiteral(
            "The account could not be created because the database operation failed."));
        return false;
    }
}

bool UserController::login(const QString& username, const QString& password)
{
    clearError();
    try
    {
        const auto user = m_userService.authenticateUser(
            username.toStdString(), password.toStdString());
        if (!user)
        {
            setErrorMessage(QStringLiteral("Invalid username or password."));
            return false;
        }

        setCurrentUser(
            user->getUserId(),
            QString::fromStdString(user->getUsername()),
            QString::fromStdString(user->getDisplayName()));
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "User authentication failed:" << error.what();
        setErrorMessage(QStringLiteral(
            "Login could not be completed because the database operation failed."));
        return false;
    }
}

bool UserController::searchUsers(const QString& query)
{
    clearError();
    try
    {
        const auto summaries = m_userService.searchUsers(
            m_currentUserId, query.toStdString());
        if (!summaries)
        {
            setErrorMessage(QStringLiteral(
                "User search is unavailable. Sign in again or shorten the search text."));
            return false;
        }

        QVariantList users;
        users.reserve(static_cast<qsizetype>(summaries->size()));
        for (const auto& summary : *summaries)
        {
            QVariantMap user;
            user.insert(
                QStringLiteral("userId"),
                QVariant::fromValue<qulonglong>(summary.getUserId()));
            user.insert(
                QStringLiteral("username"),
                QString::fromStdString(summary.getUsername()));
            user.insert(
                QStringLiteral("displayName"),
                QString::fromStdString(summary.getDisplayName()));
            users.append(user);
        }

        m_users = std::move(users);
        emit usersChanged();
        return true;
    }
    catch (const std::exception& error)
    {
        qCritical().noquote() << "User search failed:" << error.what();
        setErrorMessage(QStringLiteral(
            "Users could not be loaded because the database operation failed."));
        return false;
    }
}

void UserController::logout()
{
    clearError();
    if (!m_loggedIn && m_username.isEmpty() && m_displayName.isEmpty() &&
        m_users.isEmpty())
        return;

    m_loggedIn = false;
    m_currentUserId = 0;
    m_username.clear();
    m_displayName.clear();
    if (!m_users.isEmpty())
    {
        m_users.clear();
        emit usersChanged();
    }
    emit currentUserChanged();
    emit loggedInChanged();
}

void UserController::clearError()
{
    setErrorMessage(QString());
}

void UserController::setErrorMessage(const QString& message)
{
    if (m_errorMessage == message) return;
    m_errorMessage = message;
    emit errorMessageChanged();
}

void UserController::setCurrentUser(
    std::uint64_t userId,
    const QString& username,
    const QString& displayName)
{
    const bool identityChanged = m_currentUserId != userId ||
                                 m_username != username ||
                                 m_displayName != displayName;
    const bool loginChanged = !m_loggedIn;
    m_currentUserId = userId;
    m_username = username;
    m_displayName = displayName;
    m_loggedIn = true;
    if (identityChanged) emit currentUserChanged();
    if (loginChanged) emit loggedInChanged();
}
