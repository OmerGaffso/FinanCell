#pragma once

#include <string>
#include <cstdint>

class User 
{
    public:
        User(const std::string& username, const std::string& displayName, const std::string& password);
        User(std::uint64_t userId, const std::string& username, const std::string& displayName, const std::string& password);
        std::string getUsername() const;
        std::string getDisplayName() const;
        std::uint64_t getUserId() const;
        bool checkPassword(const std::string& password) const;

    private:
        std::string m_username;
        std::string m_displayName;
        std::uint64_t m_userId;
        std::string m_password;
};
