#pragma once

#include <cstdint>
#include <optional>

#include "application/CellService.h"
#include "application/TransactionService.h"
#include "application/UserService.h"

/** @brief Interactive console front end for FinanCell. */
class ConsoleUI
{
public:
    /** @brief Creates the UI. @param userService User service. @param cellService Cell service. @param transactionService Transaction service. */
    ConsoleUI(
        UserService& userService,
        CellService& cellService,
        TransactionService& transactionService)
        : m_userService(userService),
          m_cellService(cellService),
          m_transactionService(transactionService)
    {
    }
    /** @brief Runs the menu loop until exit or EOF. */
    void runApp();

private:
    /** @brief Prompts for and creates an account. */
    void createAccount();
    /** @brief Prompts for credentials and logs in. */
    void login();
    /** @brief Prints users for diagnostics. */
    void printUsers() const;
    /** @brief Prints cells accessible to the current user. */
    void printCells() const;
    /** @brief Prints members of the selected cell. */
    void printCellMembers();
    /** @brief Adds a member to the selected cell. */
    void addCellMember();
    /** @brief Changes a member role in the selected cell. */
    void changeCellMemberRole();
    /** @brief Removes a member from the selected cell. */
    void removeCellMember();
    /** @brief Edits the selected cell. */
    void editCell();
    /** @brief Deletes the selected cell after confirmation. */
    void deleteCell();
    /** @brief Prints optionally filtered transactions. */
    void printTransactions();
    /** @brief Adds a transaction to the selected cell. */
    void addTransaction();
    /** @brief Edits a transaction. */
    void editTransaction();
    /** @brief Deletes a transaction. */
    void deleteTransaction();
    /** @brief Prints balances and financial summaries. */
    void printCellBalance();
    /** @brief Prints the unauthenticated menu. */
    void displayMainMenu() const;
    /** @brief Prints the authenticated user menu. */
    void displayUserActionMenu() const;
    /** @brief Prints the selected-cell menu. */
    void displayCellActionMenu() const;
    /** @brief Selects an accessible cell. */
    void selectCell();
    /** @brief Runs a membership-management action. */
    void manageMembers();
    /** @brief Reads a menu choice. @param choice Parsed choice. @return False on EOF. */
    bool readChoice(int& choice) const;
    /** @brief Reads a hidden password. @param prompt Prompt text. @param password Captured password. @return False on EOF. */
    bool readPassword(const std::string& prompt, std::string& password) const;
    /** @brief Reads a positive ID. @param prompt Prompt text. @param value Parsed ID. @return True when valid. */
    bool readId(const std::string& prompt, std::uint64_t& value) const;
    /** @brief Reads a cell role. @param role Parsed role. @return True when valid. */
    bool readRole(CellRole& role) const;
    /** @brief Reads a transaction type. @param type Parsed type. @return True when valid. */
    bool readTransactionType(TransactionType& type) const;
    /** @brief Reads an exact monetary amount. @param amountInMinorUnits Parsed minor units. @return True when valid. */
    bool readAmount(std::int64_t& amountInMinorUnits) const;
    /** @brief Formats money. @param amountInMinorUnits Signed minor units. @return Formatted ILS amount. */
    std::string formatMoney(std::int64_t amountInMinorUnits) const;
    /** @brief Returns the current user's role in the selected cell. @return Role, or empty when inaccessible. */
    std::optional<CellRole> currentCellRole() const;
    /** @brief Checks for whitespace. @param text Input text. @return True when whitespace exists. */
    bool containsWhitespace(const std::string& text) const;

    /** @brief Validates a username. @param username Username. @return True when valid and available. */
    bool validateUsername(const std::string& username) const;
    /** @brief Validates a display name. @param displayName Display name. @return True when valid. */
    bool validateDisplayName(const std::string& displayName) const;
    /** @brief Validates a password. @param password Password. @return True when valid. */
    bool validatePassword(const std::string& password) const;

    /** @brief Validates a cell name. @param cellName Cell name. @return True when valid. */
    bool validateCellName(const std::string& cellName) const;
    /** @brief Validates a cell description. @param cellDescription Description. @return True when valid. */
    bool validateCellDescription(const std::string& cellDescription) const;

    /** @brief Prompts for and creates a cell. */
    void createCell();

    UserService& m_userService;
    CellService& m_cellService;
    TransactionService& m_transactionService;
    std::uint64_t m_currentUserId = 0; // Track the current user ID
    std::uint64_t m_currentCellId = 0;
};
