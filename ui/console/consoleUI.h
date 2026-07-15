#pragma once

class consoleUI
{
public:
    void run_app();

private:
    void display_main_menu() const;
    int read_choice() const;
};