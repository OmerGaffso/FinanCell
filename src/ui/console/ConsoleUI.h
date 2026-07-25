#pragma once

#include <cstdint>

#include "application/UserService.h"

class ConsoleUI
{
public:
    explicit ConsoleUI(UserService& userService) : m_userService(userService) {}
    void runApp();

private:
    void createAccount();
    void login();
    void printUsers() const;
    void displayMainMenu() const;
    void displayUserActionMenu() const;
    bool readChoice(int& choice) const;
    bool containsWhitespace(const std::string& text) const;

    bool validateUsername(const std::string& username) const;
    bool validateDisplayName(const std::string& displayName) const;
    bool validatePassword(const std::string& password) const;

    UserService& m_userService;
    std::uint64_t m_currentUserId = 0; // Track the current user ID
};
