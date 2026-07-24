#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "../../domain/User.h"

class UserService 
{
    public:
        static constexpr std::size_t MIN_USERNAME_LENGTH = 3;
        static constexpr std::size_t MAX_USERNAME_LENGTH = 30;
        static constexpr std::size_t MIN_DISPLAY_NAME_LENGTH = 3;
        static constexpr std::size_t MAX_DISPLAY_NAME_LENGTH = 50;
        static constexpr std::size_t MIN_PASSWORD_LENGTH = 6;
        static constexpr std::size_t MAX_PASSWORD_LENGTH = 18;

        UserService();
        bool createUser(std::string& username, std::string& displayName, std::string& password);
        bool userExists(const std::string& username) const;
        bool isUsernameLengthValid(const std::string& username) const;
        bool isDisplayNameLengthValid(const std::string& displayName) const;
        bool isPasswordLengthValid(const std::string& password) const;

        const User* authenticateUser(const std::string& username, const std::string& password);
        const User* findUserByUsername(const std::string& username) const;

        void printUsers() const;

    private:
        std::string normalizeText(const std::string& text) const;
        std::string trim(const std::string& str) const;
        std::string toLower(const std::string& str) const;

        std::vector<User> m_users;
};
