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
                std::cout << "GoodBye!\n" << std::endl;
                isRunning = false;
                break;
            case 1:
                std::cout << "Cannot create account yet.\n" << std::endl;
                break;
            case 2:
                std::cout << "Cannot create a cell yet.\n" << std::endl;
            
            default:
                break;
        }
    }
}

void consoleUI::display_main_menu() const
{
    std::cout << "==============" << std::endl;
    std::cout << "FinanCell App" << std::endl;
    std::cout << "==============" << std::endl;
    std::cout << "1. Create Account" << std::endl;
    std::cout << "2. Create Cell" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "==============" << std::endl;
}

int consoleUI::read_choice() const
{
    int choice;
    std::cout << "Enter your choice: ";
    std::cin >> choice;

    // Check for invalid input
    if (std::cin.fail())
    {
        std::cin.clear(); // Clear the error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        std::cout << "Invalid input. Please enter a number." << std::endl;
        return -1; // Return an invalid choice
    }

    return choice;
}