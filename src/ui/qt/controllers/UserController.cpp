#include "ui/qt/controllers/UserController.h"

#include <QDebug>

#include <exception>
#include <string>

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

void UserController::logout()
{
    clearError();
    if (!m_loggedIn && m_username.isEmpty() && m_displayName.isEmpty()) return;

    m_loggedIn = false;
    m_username.clear();
    m_displayName.clear();
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
    const QString& username,
    const QString& displayName)
{
    const bool identityChanged = m_username != username || m_displayName != displayName;
    const bool loginChanged = !m_loggedIn;
    m_username = username;
    m_displayName = displayName;
    m_loggedIn = true;
    if (identityChanged) emit currentUserChanged();
    if (loginChanged) emit loggedInChanged();
}
