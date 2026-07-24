#include "User.h"

User::User(const std::string& username, const std::string& password) : m_username(username), m_password(password)
{
    static uint8_t nextUserID = 1; // Static variable to keep track of the next user ID
    m_userID = nextUserID++; // Assign the current value and then increment for the
}

std::string User::getUsername() const
{
    return m_username;
}

uint8_t User::getUserID() const
{
    return m_userID;
}

bool User::checkPassword(const std::string& password) const
{
    return m_password == password;
}
