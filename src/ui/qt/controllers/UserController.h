#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class UserService;
class SessionState;

/** @brief Qt-facing adapter for registration, authentication, and GUI session state. */
class UserController final : public QObject
{
    Q_OBJECT
    /** @brief Whether the current GUI session is authenticated. */
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    /** @brief Normalized username for the current GUI session. */
    Q_PROPERTY(QString username READ username NOTIFY currentUserChanged)
    /** @brief Display name for the current GUI session. */
    Q_PROPERTY(QString displayName READ displayName NOTIFY currentUserChanged)
    /** @brief Latest user-facing account-operation error. */
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    /** @brief Public user summaries returned by the latest directory search. */
    Q_PROPERTY(QVariantList users READ users NOTIFY usersChanged)

public:
    /** @brief Creates the controller. @param userService Existing user application service. @param session Shared GUI session. @param parent Optional Qt owner. */
    UserController(
        UserService& userService,
        SessionState& session,
        QObject* parent = nullptr);

    /** @brief Returns whether the GUI session is authenticated. @return True after a successful login. */
    bool loggedIn() const;
    /** @brief Returns the current normalized username. @return Username, or empty while logged out. */
    QString username() const;
    /** @brief Returns the current display name. @return Display name, or empty while logged out. */
    QString displayName() const;
    /** @brief Returns the latest user-facing failure. @return Error text, or empty when no error is active. */
    QString errorMessage() const;
    /** @brief Returns public results from the latest directory search. @return User summary maps for QML. */
    QVariantList users() const;

    /** @brief Registers an account through UserService. @param username Requested username. @param displayName Requested display name. @param password Plaintext password used only for this call. @return True when the account was created. */
    Q_INVOKABLE bool registerUser(
        const QString& username,
        const QString& displayName,
        const QString& password);
    /** @brief Authenticates through UserService. @param username Submitted username. @param password Plaintext password used only for this call. @return True when authenticated. */
    Q_INVOKABLE bool login(const QString& username, const QString& password);
    /** @brief Searches registered users through UserService. @param query Optional username or display-name fragment. @return True when the search completed. */
    Q_INVOKABLE bool searchUsers(const QString& query);
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
    /** @brief Emitted when directory search results change. */
    void usersChanged();

private:
    void setErrorMessage(const QString& message);
    void setCurrentUser(
        std::uint64_t userId,
        const QString& username,
        const QString& displayName);

    UserService& m_userService;
    SessionState& m_session;
    QString m_errorMessage;
    QVariantList m_users;
};
