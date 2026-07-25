#include "ui/console/ConsoleUI.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>

void ConsoleUI::runApp()
{
    bool isRunning = true;
    while(isRunning)
    {
        if (m_currentUserId == 0)
        {
            displayMainMenu();
        }
        else
        {
            displayUserActionMenu();
        }
        int choice;
        if (!readChoice(choice))
        {
            std::cout << "\nInput closed. Goodbye!\n";
            break;
        }

        if (m_currentUserId == 0)
        {
            switch (choice)
            {
                case 0: 
                    std::cout << "GoodBye!\n" << std::endl;
                    isRunning = false;
                    break;
                case 1:
                    createAccount();
                    break;
                case 2:
                    login();
                    break;
                case 3:
                    printUsers();
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
                    m_currentUserId = 0; // Reset current user ID on logout
                    break;
                case 1:
                    std::cout << "Create Cell functionality is not implemented yet.\n" << std::endl;
                    break;
                case 2:
                    printCells();
                    break;
                default:
                    std::cout << "Please select a valid menu option.\n" << std::endl;
                    break;
            }
        }
    }
}

void ConsoleUI::createAccount()
{
    std::string username;
    std::string displayName;
    std::string password;

    std::cout << "Username: ";
    std::getline(std::cin, username);
    if (!validateUsername(username))
    {
        std::cout << "Invalid username.\n" << std::endl;
        return;
    }

    std::cout << "Display name: ";
    std::getline(std::cin, displayName);

    if (!validateDisplayName(displayName))
    {
        std::cout << "Invalid display name.\n" << std::endl;
        return;
    }

    std::cout << "Password: ";
    std::getline(std::cin, password);

    if (!validatePassword(password))
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

void ConsoleUI::login()
{
    std::string username;
    std::string password;

    std::cout << "Username: ";
    std::getline(std::cin, username);

    std::cout << "Password: ";
    std::getline(std::cin, password);

    if (const std::optional<User> user = m_userService.authenticateUser(username, password))
    {
        m_currentUserId = user->getUserId();
        std::cout << "Login successful. Welcome, " << user->getDisplayName() << "!\n" << std::endl;
    }
    else
    {
        std::cout << "Invalid username or password.\n" << std::endl;
    }
}

void ConsoleUI::printUsers() const
{
    std::cout << "\nCurrent users:" << std::endl;

    const std::vector<User> users = m_userService.getUsers();
    if (users.empty())
    {
        std::cout << "No users have been created.\n";
    }
    else
    {
        for (const User& user : users)
        {
            std::cout << "ID: " << user.getUserId()
                      << ", Username: " << user.getUsername()
                      << ", Display Name: " << user.getDisplayName()
                      << '\n';
        }
    }

    std::cout << std::endl;
}

void ConsoleUI::printCells() const
{
    std::cout << "\nCurrent cells:" << std::endl;

    const std::vector<FinancialCell> cells = m_cellService.getCells();
    if (cells.empty())
    {
        std::cout << "No cells have been created.\n";
    }
    else
    {
        for (const FinancialCell& cell : cells)
        {
            std::cout << "ID: " << cell.getCellId()
                      << ", Name: " << cell.getCellName()
                      << ", Description: " << cell.getCellDescription()
                      << ", Currency: " << cell.getUsesCurrency()
                      << ", Owner ID: " << cell.getOwnerId()
                      << '\n';
        }
    }

    std::cout << std::endl;
}

void ConsoleUI::displayMainMenu() const
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

void ConsoleUI::displayUserActionMenu() const
{
    std::cout << "==============" << std::endl;
    std::cout << "User Actions" << std::endl;
    std::cout << "==============" << std::endl;
    std::cout << "1. Create Cell" << std::endl;
    std::cout << "2. View Cells" << std::endl;
    std::cout << "0. Logout" << std::endl;
    std::cout << "==============" << std::endl;
}

bool ConsoleUI::readChoice(int& choice) const
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

bool ConsoleUI::containsWhitespace(const std::string& text) const
{
    return std::any_of(
        text.begin(),
        text.end(),
        [](unsigned char character)
        {
            return std::isspace(character);
        });
}

bool ConsoleUI::validateUsername(const std::string& username) const
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
    if (containsWhitespace(username))
    {
        std::cout << "Username cannot contain spaces.\n" << std::endl;
        return false;
    }

    return true;
}

bool ConsoleUI::validateDisplayName(const std::string& displayName) const
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

bool ConsoleUI::validatePassword(const std::string& password) const
{    if (!m_userService.isPasswordLengthValid(password))
    {
        std::cout << "Password must be between "
                  << UserService::MIN_PASSWORD_LENGTH << " and "
                  << UserService::MAX_PASSWORD_LENGTH
                  << " characters.\n" << std::endl;
        return false;
    }
    if (containsWhitespace(password))
    {
        std::cout << "Password cannot contain spaces.\n" << std::endl;
        return false;
    }
    return true;
}
