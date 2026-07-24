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
                create_account();
                break;
            case 2:
                print_users();
                break;
            case 3:
                std::cout << "Cannot create a cell yet.\n" << std::endl;
                break;
            default:
                std::cout << "Please select a valid menu option.\n" << std::endl;
                break;
        }
    }
}

void consoleUI::create_account()
{
    std::string username;
    std::string displayName;
    std::string password;

    std::cout << "Username: ";
    std::getline(std::cin, username);
    
    if (username.empty())
    {
        std::cout << "Username cannot be empty.\n" << std::endl;
        return;
    }
    if (m_userService.userExists(username))
    {
        std::cout << "User already exists.\n" << std::endl;
        return;
    }
    if (!checkSpaces(username))
    {
        std::cout << "Username cannot contain spaces.\n" << std::endl;
        return;
    }
    
    std::cout << "Display name: ";
    std::getline(std::cin, displayName);

    if (displayName.empty())
    {
        std::cout << "Display name cannot be empty.\n" << std::endl;
        return;
    }

    std::cout << "Password: ";
    std::getline(std::cin, password);

    if (password.empty())
    {
        std::cout << "Password cannot be empty.\n" << std::endl;
        return;
    }
    if (!checkSpaces(password))
    {
        std::cout << "Password cannot contain spaces.\n" << std::endl;
        return;
    }

    if (m_userService.createUser(username, displayName, password))
    {
        std::cout << "Account created successfully.\n" << std::endl;
    }
    else
    {
        std::cout << "Could not create the account.\n" << std::endl;
    }
}

void consoleUI::print_users() const
{
    std::cout << "\nCurrent users:" << std::endl;
    m_userService.printUsers();
    std::cout << std::endl;
}

void consoleUI::display_main_menu() const
{
    std::cout << "==============" << std::endl;
    std::cout << "FinanCell App" << std::endl;
    std::cout << "==============" << std::endl;
    std::cout << "1. Create Account" << std::endl;
    std::cout << "2. Print Users" << std::endl;
    std::cout << "3. Create Cell" << std::endl;
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

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

bool consoleUI::checkSpaces(const std::string& text) const
{
    return text.find(' ') == std::string::npos;
}
