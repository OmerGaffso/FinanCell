#include "application/UserService.h"

#include <string>

#include "utils/StringUtils.h"

UserService::UserService(UserRepository& userRepository)
    : m_userRepository(userRepository)
{
}

bool UserService::createUser(std::string& username, std::string& displayName, std::string& password)
{
    username = StringUtils::normalize(username);
    displayName = StringUtils::trim(displayName);
    password = StringUtils::trim(password);

    if (!isUsernameLengthValid(username) ||
        !isDisplayNameLengthValid(displayName) ||
        !isPasswordLengthValid(password) ||
        userExists(username))
    {
        return false;
    }

    return m_userRepository.insertUser(username, displayName, password);
}

std::optional<User> UserService::authenticateUser(
    const std::string& username,
    const std::string& password) const
{
    const std::string normalizedUsername = StringUtils::normalize(username);
    std::optional<User> user = m_userRepository.findUserByUsername(normalizedUsername);

    if (!user || !user->checkPassword(password))
    {
        return std::nullopt;
    }

    return user;
}

std::optional<User> UserService::findUserByUsername(
    const std::string& username) const
{
    return m_userRepository.findUserByUsername(StringUtils::normalize(username));
}

bool UserService::userExists(const std::string& username) const
{
    const std::string normalizedUsername = StringUtils::normalize(username);

    return m_userRepository.userExists(normalizedUsername);
}

bool UserService::isUsernameLengthValid(const std::string& username) const
{
    const std::size_t length = StringUtils::normalize(username).length();
    return length >= MIN_USERNAME_LENGTH && length <= MAX_USERNAME_LENGTH;
}

bool UserService::isDisplayNameLengthValid(const std::string& displayName) const
{
    const std::size_t length = StringUtils::trim(displayName).length();
    return length >= MIN_DISPLAY_NAME_LENGTH && length <= MAX_DISPLAY_NAME_LENGTH;
}

bool UserService::isPasswordLengthValid(const std::string& password) const
{
    const std::size_t length = StringUtils::trim(password).length();
    return length >= MIN_PASSWORD_LENGTH && length <= MAX_PASSWORD_LENGTH;
}

std::vector<User> UserService::getUsers() const
{
    return m_userRepository.findAllUsers();
}
