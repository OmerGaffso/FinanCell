#pragma once

#include "../../services/user/UserService.h"

class consoleUI
{
public:
    void run_app();

private:
    void create_account();
    void print_users() const;
    void display_main_menu() const;
    int read_choice() const;

    UserService m_userService;
};
