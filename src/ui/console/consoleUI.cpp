#include "ui/console/consoleUI.h"
#include <iostream>
#include <limits>

void consoleUI::run_app()
{
    bool isRunning = true;
    while(isRunning)
    {
        if (m_currentUserID == 0)
        {
            display_main_menu();
        }
        else
        {
            display_user_action_menu();
        }
        int choice;
        if (!read_choice(choice))
        {
            std::cout << "\nInput closed. Goodbye!\n";
            break;
        }

        if (m_currentUserID ==0)
        {
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
                    account_login();
                    break;
                case 3:
                    print_users();
                    break;
                default:
                    std::cout << "Please select a valid menu option.\n" << std::endl;
                    break;
            }
        }
        else
        {
            switch (choice)
            {
                case 0:
                    std::cout << "Logging out...\n" << std::endl;
                    m_currentUserID = 0; // Reset current user ID on logout
                    break;
                case 1:
                    std::cout << "Create Cell functionality is not implemented yet.\n" << std::endl;
                    break;
                case 2:
                    std::cout << "View Cells functionality is not implemented yet.\n" << std::endl;
                    break;
                default:
                    std::cout << "Please select a valid menu option.\n" << std::endl;
                    break;
            }
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
    if (!validate_username(username))
    {
        std::cout << "Invalid username.\n" << std::endl;
        return;
    }

    std::cout << "Display name: ";
    std::getline(std::cin, displayName);

    if (!validate_display_name(displayName))
    {
        std::cout << "Invalid display name.\n" << std::endl;
        return;
    }

    std::cout << "Password: ";
    std::getline(std::cin, password);

    if (!validate_password(password))
    {
        std::cout << "Invalid password.\n" << std::endl;
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

void consoleUI::account_login()
{
    std::string username;
    std::string password;

    std::cout << "Username: ";
    std::getline(std::cin, username);

    std::cout << "Password: ";
    std::getline(std::cin, password);

    if (const User* user = m_userService.authenticateUser(username, password))
    {
        m_currentUserID = user->getUserID();
        std::cout << "Login successful. Welcome, " << user->getDisplayName() << "!\n" << std::endl;
    }
    else
    {
        std::cout << "Invalid username or password.\n" << std::endl;
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
    std::cout << "2. Login" << std::endl;
    std::cout << "3. Print Users" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "==============" << std::endl;
}

void consoleUI::display_user_action_menu() const
{
    std::cout << "==============" << std::endl;
    std::cout << "User Actions" << std::endl;
    std::cout << "==============" << std::endl;
    std::cout << "1. Create Cell" << std::endl;
    std::cout << "2. View Cells" << std::endl;
    std::cout << "0. Logout" << std::endl;
    std::cout << "==============" << std::endl;
}

bool consoleUI::read_choice(int& choice) const
{
    std::cout << "Enter your choice: ";
    std::cin >> choice;

    if (std::cin.fail())
    {
        if (std::cin.eof())
        {
            return false;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number." << std::endl;
        choice = -1;
        return true;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

bool consoleUI::checkSpaces(const std::string& text) const
{
    return  (text.find(' ') == std::string::npos) || (text.find('\t') == std::string::npos) || 
            (text.find('\n') == std::string::npos);
}

bool consoleUI::validate_username(const std::string& username) const
{
    if (!m_userService.isUsernameLengthValid(username))
    {
        std::cout << "Username must be between "
                  << UserService::MIN_USERNAME_LENGTH << " and "
                  << UserService::MAX_USERNAME_LENGTH
                  << " characters.\n" << std::endl;
        return false;
    }
    if (m_userService.userExists(username))
    {
        std::cout << "User already exists.\n" << std::endl;
        return false;
    }
    if (!checkSpaces(username))
    {
        std::cout << "Username cannot contain spaces.\n" << std::endl;
        return false;
    }

    return true;
}

bool consoleUI::validate_display_name(const std::string& displayName) const
{
    if (!m_userService.isDisplayNameLengthValid(displayName))
    {
        std::cout << "Display name must be between "
                  << UserService::MIN_DISPLAY_NAME_LENGTH << " and "
                  << UserService::MAX_DISPLAY_NAME_LENGTH
                  << " characters.\n" << std::endl;
        return false;
    }
    return true;
}

bool consoleUI::validate_password(const std::string& password) const
{    if (!m_userService.isPasswordLengthValid(password))
    {
        std::cout << "Password must be between "
                  << UserService::MIN_PASSWORD_LENGTH << " and "
                  << UserService::MAX_PASSWORD_LENGTH
                  << " characters.\n" << std::endl;
        return false;
    }
    if (!checkSpaces(password))
    {
        std::cout << "Password cannot contain spaces.\n" << std::endl;
        return false;
    }
    return true;
}
