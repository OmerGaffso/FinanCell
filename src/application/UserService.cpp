#include "application/UserService.h"

#include <string>

#include "utils/StringUtils.h"

UserService::UserService(UserRepository& userRepository, PasswordHasher& passwordHasher)
    : m_userRepository(userRepository), m_passwordHasher(passwordHasher)
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

    return m_userRepository.insertUser(username, displayName, m_passwordHasher.hash(password));
}

std::optional<User> UserService::authenticateUser(
    const std::string& username,
    const std::string& password) const
{
    const std::string normalizedUsername = StringUtils::normalize(username);
    std::optional<User> user = m_userRepository.findUserByUsername(normalizedUsername);

    if (!user)
    {
        return std::nullopt;
    }

    const std::string& stored = user->getPasswordHash();
    if (m_passwordHasher.isEncodedHash(stored))
    {
        if (!m_passwordHasher.verify(password, stored)) return std::nullopt;
    }
    else
    {
        if (stored != password) return std::nullopt;
        m_userRepository.updatePasswordHash(user->getUserId(), m_passwordHasher.hash(password));
    }

    return user;
}

std::optional<User> UserService::findUserByUsername(
    const std::string& username) const
{
    return m_userRepository.findUserByUsername(StringUtils::normalize(username));
}

std::optional<User> UserService::findUserById(std::uint64_t userId) const
{
    return m_userRepository.findUserById(userId);
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
