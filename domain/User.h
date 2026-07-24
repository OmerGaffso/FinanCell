#pragma once

#include <string>

class User 
{
    public:
        User(const std::string& username, const std::string& displayName, const std::string& password);
        std::string getUsername() const;
        std::string getDisplayName() const;
        uint16_t getUserID() const;
        bool checkPassword(const std::string& password) const;

    private:
        std::string m_username;
        std::string m_displayName;
        uint16_t m_userID;
        std::string m_password;
};
