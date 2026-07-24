#pragma once

#include <string>

class User 
{
    public:
        User(const std::string& username, const std::string& displayName, const std::string& password);
        std::string getUsername() const;
        std::string getDisplayName() const;
        uint8_t getUserID() const;
        bool checkPassword(const std::string& password) const;

    private:
        std::string m_username;
        std::string m_displayName;
        uint8_t m_userID;
        std::string m_password;
};
