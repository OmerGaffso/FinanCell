#pragma once

#include <cstdint>

#include "application/CellService.h"
#include "application/TransactionService.h"
#include "application/UserService.h"

class ConsoleUI
{
public:
    ConsoleUI(
        UserService& userService,
        CellService& cellService,
        TransactionService& transactionService)
        : m_userService(userService),
          m_cellService(cellService),
          m_transactionService(transactionService)
    {
    }
    void runApp();

private:
    void createAccount();
    void login();
    void printUsers() const;
    void printCells() const;
    void printCellMembers();
    void addCellMember();
    void changeCellMemberRole();
    void removeCellMember();
    void editCell();
    void deleteCell();
    void printTransactions();
    void addTransaction();
    void editTransaction();
    void deleteTransaction();
    void printCellBalance();
    void displayMainMenu() const;
    void displayUserActionMenu() const;
    bool readChoice(int& choice) const;
    bool readId(const std::string& prompt, std::uint64_t& value) const;
    bool readRole(CellRole& role) const;
    bool readTransactionType(TransactionType& type) const;
    bool readAmount(std::int64_t& amountInMinorUnits) const;
    std::string formatMoney(std::int64_t amountInMinorUnits) const;
    bool containsWhitespace(const std::string& text) const;

    bool validateUsername(const std::string& username) const;
    bool validateDisplayName(const std::string& displayName) const;
    bool validatePassword(const std::string& password) const;

    bool validateCellName(const std::string& cellName) const;
    bool validateCellDescription(const std::string& cellDescription) const;

    void createCell();

    UserService& m_userService;
    CellService& m_cellService;
    TransactionService& m_transactionService;
    std::uint64_t m_currentUserId = 0; // Track the current user ID
};
