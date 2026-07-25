#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "application/UserRepository.h"
#include "domain/User.h"

class UserService 
{
    public:
        static constexpr std::size_t MIN_USERNAME_LENGTH = 3;
        static constexpr std::size_t MAX_USERNAME_LENGTH = 30;
        static constexpr std::size_t MIN_DISPLAY_NAME_LENGTH = 3;
        static constexpr std::size_t MAX_DISPLAY_NAME_LENGTH = 50;
        static constexpr std::size_t MIN_PASSWORD_LENGTH = 6;
        static constexpr std::size_t MAX_PASSWORD_LENGTH = 18;

        explicit UserService(UserRepository& userRepository);
        bool createUser(std::string& username, std::string& displayName, std::string& password);
        bool userExists(const std::string& username) const;
        bool isUsernameLengthValid(const std::string& username) const;
        bool isDisplayNameLengthValid(const std::string& displayName) const;
        bool isPasswordLengthValid(const std::string& password) const;

        std::optional<User> authenticateUser(const std::string& username, const std::string& password) const;
        std::vector<User> getUsers() const;

    private:
        UserRepository& m_userRepository;
};
