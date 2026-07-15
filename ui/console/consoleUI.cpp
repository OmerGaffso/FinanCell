#include "consoleUI.h"
#include <iostream>
#include <limits>

void consoleUI::run_app()
{
    bool isRunning = true;
    while(isRunning)
    {
        display_main_menu();
        int choice = read_choice();
        switch (choice)
        {
            case 0: 
                std::cout << "GoodBye!" << std::endl;
                isRunning = false;
                break;
            case 1:
                std::cout << "Cannot create account yet." << std::endl;
                break;
            case 2:
                std::cout << "Cannot create a cell yet." << std::endl;
            
            default:
                break;
        }
    }
}

void consoleUI::display_main_menu() const
{
    
}

int consoleUI::read_choice() const
{
    static int i = 1;
    if (i == 3)
        i = 0;
    return i++;
}