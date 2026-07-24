#include <string>
#include <vector>
#include "../../domain/User.h"

#ifndef USER_SERVICE_H
#define USER_SERVICE_H

class UserService 
{
    public:
        UserService();
        bool createUser(const std::string& username, const std::string& displayName, const std::string& password);
        bool authenticateUser(const std::string& username, const std::string& password);
        void printUsers() const;

    private:
        std::vector<User> m_users;
};

#endif // USER_SERVICE_H
