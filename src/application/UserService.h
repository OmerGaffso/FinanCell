#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "application/UserRepository.h"
#include "domain/User.h"
#include "security/PasswordHasher.h"

/** @brief Handles account validation, password hashing, and authentication. */
class UserService
{
    public:
        /** @brief Minimum accepted username length. */
        static constexpr std::size_t MIN_USERNAME_LENGTH = 3;
        /** @brief Maximum accepted username length. */
        static constexpr std::size_t MAX_USERNAME_LENGTH = 30;
        /** @brief Minimum accepted display-name length. */
        static constexpr std::size_t MIN_DISPLAY_NAME_LENGTH = 3;
        /** @brief Maximum accepted display-name length. */
        static constexpr std::size_t MAX_DISPLAY_NAME_LENGTH = 50;
        /** @brief Minimum accepted password length. */
        static constexpr std::size_t MIN_PASSWORD_LENGTH = 6;
        /** @brief Maximum accepted password length. */
        static constexpr std::size_t MAX_PASSWORD_LENGTH = 18;

        /** @brief Creates the service. @param userRepository User persistence. @param passwordHasher Password hashing provider. */
        UserService(UserRepository& userRepository, PasswordHasher& passwordHasher);
        /** @brief Creates a user. @param username Username to normalize. @param displayName Display name to trim. @param password Plaintext password to hash. @return True on success. */
        bool createUser(std::string& username, std::string& displayName, std::string& password);
        /** @brief Checks username existence. @param username Username. @return True when registered. */
        bool userExists(const std::string& username) const;
        /** @brief Validates username length. @param username Username. @return True when valid. */
        bool isUsernameLengthValid(const std::string& username) const;
        /** @brief Validates display-name length. @param displayName Display name. @return True when valid. */
        bool isDisplayNameLengthValid(const std::string& displayName) const;
        /** @brief Validates password length. @param password Password. @return True when valid. */
        bool isPasswordLengthValid(const std::string& password) const;

        /** @brief Authenticates a user. @param username Username. @param password Plaintext password. @return Authenticated user, or empty. */
        std::optional<User> authenticateUser(const std::string& username, const std::string& password) const;
        /** @brief Finds a user by username. @param username Username. @return Matching user, or empty. */
        std::optional<User> findUserByUsername(const std::string& username) const;
        /** @brief Finds a user by ID. @param userId User ID. @return Matching user, or empty. */
        std::optional<User> findUserById(std::uint64_t userId) const;
        /** @brief Returns every user. @return All users. */
        std::vector<User> getUsers() const;

    private:
        UserRepository& m_userRepository;
        PasswordHasher& m_passwordHasher;
};
