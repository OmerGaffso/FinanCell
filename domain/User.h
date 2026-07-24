#ifndef USER_H
#define USER_H
#include <string>

class User 
{
    public:
        User(const std::string& username, const std::string& password);
        std::string getUsername() const;
        uint8_t getUserID() const;
        bool checkPassword(const std::string& password) const;

    private:
        std::string m_username;
        uint8_t m_userID;
        std::string m_password;
};

#endif USER_H