#pragma once

#include <QObject>
#include <QString>

class UserService;

/** @brief Qt-facing adapter for registration, authentication, and GUI session state. */
class UserController final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString username READ username NOTIFY currentUserChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY currentUserChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    /** @brief Creates the controller. @param userService Existing user application service. @param parent Optional Qt owner. */
    explicit UserController(UserService& userService, QObject* parent = nullptr);

    /** @brief Returns whether the GUI session is authenticated. @return True after a successful login. */
    bool loggedIn() const;
    /** @brief Returns the current normalized username. @return Username, or empty while logged out. */
    QString username() const;
    /** @brief Returns the current display name. @return Display name, or empty while logged out. */
    QString displayName() const;
    /** @brief Returns the latest user-facing failure. @return Error text, or empty when no error is active. */
    QString errorMessage() const;

    /** @brief Registers an account through UserService. @param username Requested username. @param displayName Requested display name. @param password Plaintext password used only for this call. @return True when the account was created. */
    Q_INVOKABLE bool registerUser(
        const QString& username,
        const QString& displayName,
        const QString& password);
    /** @brief Authenticates through UserService. @param username Submitted username. @param password Plaintext password used only for this call. @return True when authenticated. */
    Q_INVOKABLE bool login(const QString& username, const QString& password);
    /** @brief Clears the current GUI session. */
    Q_INVOKABLE void logout();
    /** @brief Clears the current user-facing error. */
    Q_INVOKABLE void clearError();

signals:
    /** @brief Emitted when the authentication state changes. */
    void loggedInChanged();
    /** @brief Emitted when current user identity fields change. */
    void currentUserChanged();
    /** @brief Emitted when the user-facing error changes. */
    void errorMessageChanged();

private:
    void setErrorMessage(const QString& message);
    void setCurrentUser(const QString& username, const QString& displayName);

    UserService& m_userService;
    bool m_loggedIn{false};
    QString m_username;
    QString m_displayName;
    QString m_errorMessage;
};
