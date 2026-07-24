#pragma once

#include "../../services/user/UserService.h"

class consoleUI
{
public:
    void run_app();

private:
    void create_account();
    void account_login();
    void print_users() const;
    void display_main_menu() const;
    void display_user_action_menu() const;
    bool read_choice(int& choice) const;
    bool checkSpaces(const std::string& text) const; // used to validate that passwords and usernames do not contain spaces

    bool validate_username(const std::string& username) const;
    bool validate_display_name(const std::string& displayName) const;
    bool validate_password(const std::string& password) const;

    UserService m_userService;
    uint16_t m_currentUserID = 0; // Track the current user ID
};
