#include "application/UserService.h"
#include <string>

UserService::UserService(UserRepository& userRepository)
    : m_userRepository(userRepository)
{
}

bool UserService::createUser(std::string& username, std::string& displayName, std::string& password)
{
    username = normalizeText(username);
    displayName = trim(displayName);
    password = trim(password);

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
    const std::string normalizedUsername = normalizeText(username);
    std::optional<User> user = m_userRepository.findUserByUsername(normalizedUsername);

    if (!user || !user->checkPassword(password))
    {
        return std::nullopt;
    }

    return user;
}

bool UserService::userExists(const std::string& username) const
{
    const std::string normalizedUsername = normalizeText(username);

    return m_userRepository.userExists(normalizedUsername);
}

bool UserService::isUsernameLengthValid(const std::string& username) const
{
    const std::size_t length = normalizeText(username).length();
    return length >= MIN_USERNAME_LENGTH && length <= MAX_USERNAME_LENGTH;
}

bool UserService::isDisplayNameLengthValid(const std::string& displayName) const
{
    const std::size_t length = trim(displayName).length();
    return length >= MIN_DISPLAY_NAME_LENGTH && length <= MAX_DISPLAY_NAME_LENGTH;
}

bool UserService::isPasswordLengthValid(const std::string& password) const
{
    const std::size_t length = trim(password).length();
    return length >= MIN_PASSWORD_LENGTH && length <= MAX_PASSWORD_LENGTH;
}

std::vector<User> UserService::getUsers() const
{
    return m_userRepository.findAllUsers();
}

std::string UserService::normalizeText(const std::string& text) const
{
    return toLower(trim(text));
}

std::string UserService::trim(const std::string& str) const
{
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start != std::string::npos && end != std::string::npos)
    {
        return str.substr(start, end - start + 1);
    }
    return "";
}

std::string UserService::toLower(const std::string& str) const
{
    std::string lowerStr = str;
    for (char& c : lowerStr)
    {
        c = std::tolower(c);
    }
    return lowerStr;
}
