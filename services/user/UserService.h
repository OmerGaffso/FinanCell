#pragma once

#include <string>
#include <vector>
#include "../../domain/User.h"

class UserService 
{
    public:
        UserService();
        bool createUser(std::string& username, std::string& displayName, std::string& password);
        bool authenticateUser(const std::string& username, const std::string& password);
        bool userExists(const std::string& username) const;
        void printUsers() const;

    private:
        std::string normalizeText(const std::string& text) const;
        std::string trim(const std::string& str) const;
        std::string toLower(const std::string& str) const;

        std::vector<User> m_users;
};
