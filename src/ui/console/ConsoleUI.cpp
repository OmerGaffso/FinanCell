#include "ui/console/ConsoleUI.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <termios.h>
#include <unistd.h>

#include "utils/StringUtils.h"

namespace
{
const char* cellResultMessage(CellOperationResult result)
{
    switch (result)
    {
        case CellOperationResult::SUCCESS: return "Operation completed successfully.";
        case CellOperationResult::CELL_NOT_FOUND: return "Cell not found.";
        case CellOperationResult::USER_NOT_FOUND: return "User not found.";
        case CellOperationResult::ALREADY_MEMBER: return "User is already a member.";
        case CellOperationResult::MEMBER_NOT_FOUND: return "Member not found.";
        case CellOperationResult::INVALID_ROLE: return "The requested role is invalid.";
        case CellOperationResult::INVALID_INPUT: return "The supplied cell data is invalid.";
        case CellOperationResult::NOT_AUTHORIZED: return "You are not authorized for this operation.";
        case CellOperationResult::CANNOT_MODIFY_OWNER: return "The cell owner cannot be modified.";
        case CellOperationResult::STORAGE_ERROR: return "The database operation failed.";
    }
    return "Unknown operation result.";
}
}

void ConsoleUI::runApp()
{
    bool isRunning = true;
    while (isRunning)
    {
        if (m_currentUserId == 0) displayMainMenu();
        else if (m_currentCellId == 0) displayUserActionMenu();
        else displayCellActionMenu();

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
                default:
                    std::cout << "Please select a valid menu option.\n" << std::endl;
                    break;
            }
        }
        else if (m_currentCellId == 0)
        {
            switch (choice)
            {
                case 0:
                case 4:
                    std::cout << "Logging out...\n" << std::endl;
                    m_currentUserId = 0;
                    break;
                case 1:
                    createCell();
                    break;
                case 2:
                    printCells();
                    break;
                case 3:
                    selectCell();
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
                case 1: printTransactions(); break;
                case 2: addTransaction(); break;
                case 3: editTransaction(); break;
                case 4: deleteTransaction(); break;
                case 5: printCellBalance(); break;
                case 6: manageMembers(); break;
                case 7: editCell(); break;
                case 8: deleteCell(); break;
                case 0:
                case 9: m_currentCellId = 0; break;
                default:
                    std::cout << "Please select a valid menu option.\n" << std::endl;
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

    if (!readPassword("Password: ", password)) return;

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

void ConsoleUI::createCell()
{
    if (m_currentUserId == 0)
    {
        std::cout << "No user connected. Cannot perform this action.\n" << std::endl;
        return;
    }

    std::string cellName;
    std::string cellDescription;

    std::cout << "Creating new cell:\n";
    std::cout << "Cell Name: ";
    std::getline(std::cin, cellName);

    if (!validateCellName(cellName))
    {
        std::cout << "Invalid cell name.\n" << std::endl;
        return;
    }

    std::cout << "Cell Description: ";
    std::getline(std::cin, cellDescription);

    if (!validateCellDescription(cellDescription))
    {
        std::cout << "Invalid cell description.\n" << std::endl;
        return;
    }
    
    if (m_cellService.createCell(cellName, m_currentUserId, cellDescription))
    {
        std::cout << "Cell " << cellName << " created successfully!\n" << std::endl;
    }
    else
    {
        std::cout << "Could not create the cell.\n" << std::endl;
    }
}

void ConsoleUI::login()
{
    std::string username;
    std::string password;

    std::cout << "Username: ";
    std::getline(std::cin, username);

    if (!readPassword("Password: ", password)) return;

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
    std::cout << "\nMy cells:" << std::endl;

    const std::vector<FinancialCell> cells = m_cellService.getCellsForUser(m_currentUserId);
    if (cells.empty())
    {
        std::cout << "You do not belong to any cells.\n";
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

void ConsoleUI::printCellMembers()
{
    const std::uint64_t cellId = m_currentCellId;

    const auto members = m_cellService.getCellMembers(m_currentUserId, cellId);
    if (members.empty())
    {
        std::cout << "Cell not found or access denied.\n" << std::endl;
        return;
    }

    std::cout << "\nCell members:\n";
    for (const CellMember& member : members)
    {
        const char* role = member.role == CellRole::OWNER ? "OWNER" :
                           member.role == CellRole::MEMBER ? "MEMBER" : "GUEST";
        const auto user = m_userService.findUserById(member.userId);
        std::cout << "User: " << (user ? user->getUsername() : "unknown")
                  << " (ID: " << member.userId << "), Role: " << role << '\n';
    }
    std::cout << std::endl;
}

void ConsoleUI::addCellMember()
{
    const std::uint64_t cellId = m_currentCellId;

    std::string username;
    std::cout << "Username to add: ";
    std::getline(std::cin, username);
    const auto user = m_userService.findUserByUsername(username);
    if (!user)
    {
        std::cout << "User not found.\n" << std::endl;
        return;
    }

    CellRole role;
    if (!readRole(role)) return;
    const auto result = m_cellService.addMemberToCell(
        m_currentUserId, cellId, user->getUserId(), role);
    std::cout << cellResultMessage(result) << '\n' << std::endl;
}

void ConsoleUI::changeCellMemberRole()
{
    const std::uint64_t cellId = m_currentCellId;
    std::uint64_t userId;
    if (!readId("Member user ID: ", userId)) return;
    CellRole role;
    if (!readRole(role)) return;

    std::cout << cellResultMessage(m_cellService.updateMemberRole(
        m_currentUserId, cellId, userId, role)) << '\n' << std::endl;
}

void ConsoleUI::removeCellMember()
{
    const std::uint64_t cellId = m_currentCellId;
    std::uint64_t userId;
    if (!readId("Member user ID: ", userId)) return;
    std::cout << cellResultMessage(m_cellService.removeMemberFromCell(
        m_currentUserId, cellId, userId)) << '\n' << std::endl;
}

void ConsoleUI::editCell()
{
    const std::uint64_t cellId = m_currentCellId;
    std::string name;
    std::string description;
    std::cout << "New cell name: ";
    std::getline(std::cin, name);
    std::cout << "New description: ";
    std::getline(std::cin, description);
    std::cout << cellResultMessage(m_cellService.updateCell(
        m_currentUserId, cellId, name, description)) << '\n' << std::endl;
}

void ConsoleUI::deleteCell()
{
    const std::uint64_t cellId = m_currentCellId;
    std::string confirmation;
    std::cout << "Type DELETE to permanently delete the cell: ";
    std::getline(std::cin, confirmation);
    if (confirmation != "DELETE")
    {
        std::cout << "Deletion cancelled.\n" << std::endl;
        return;
    }
    const auto result = m_cellService.deleteCell(m_currentUserId, cellId);
    if (result == CellOperationResult::SUCCESS)
    {
        std::cout << "Cell deleted successfully.\n" << std::endl;
        m_currentCellId = 0;
    }
    else std::cout << cellResultMessage(result) << '\n' << std::endl;
}

void ConsoleUI::printTransactions()
{
    const std::uint64_t cellId = m_currentCellId;
    std::string fromDate;
    std::string toDate;
    std::cout << "From date YYYY-MM-DD (blank for all): ";
    std::getline(std::cin, fromDate);
    if (!fromDate.empty())
    {
        std::cout << "To date YYYY-MM-DD: ";
        std::getline(std::cin, toDate);
    }
    const auto transactions =
        m_transactionService.getTransactionsForCell(m_currentUserId, cellId, fromDate, toDate);
    if (!transactions)
    {
        std::cout << "Cell not found or access denied.\n" << std::endl;
        return;
    }
    if (transactions->empty())
    {
        std::cout << "No transactions have been recorded.\n" << std::endl;
        return;
    }

    std::cout << "\nTransactions:\n";
    for (const Transaction& transaction : *transactions)
    {
        std::cout << "ID: " << transaction.getTransactionId()
                  << ", Type: "
                  << (transaction.getType() == TransactionType::INCOME ? "INCOME" : "EXPENSE")
                  << ", Amount: " << formatMoney(transaction.getAmountInMinorUnits())
                  << ", Description: " << transaction.getDescription()
                  << ", Category: " << transaction.getCategory()
                  << ", Created by user: " << transaction.getUserId()
                  << ", Date: " << transaction.getOccurredAt() << '\n';
    }
    std::cout << std::endl;
}

void ConsoleUI::addTransaction()
{
    const std::uint64_t cellId = m_currentCellId;
    TransactionType type;
    if (!readTransactionType(type)) return;
    std::string description;
    std::cout << "Description: ";
    std::getline(std::cin, description);
    std::int64_t amount;
    if (!readAmount(amount)) return;
    std::string category;
    std::string occurredAt;
    std::cout << "Category (blank for General): ";
    std::getline(std::cin, category);
    if (StringUtils::trim(category).empty()) category = "General";
    std::cout << "Date YYYY-MM-DD (blank for today): ";
    std::getline(std::cin, occurredAt);

    const auto transaction = m_transactionService.addTransaction(
        m_currentUserId, cellId, type, description, amount, occurredAt, category);
    std::cout << (transaction ? "Transaction added successfully.\n\n"
                              : "Could not add transaction.\n\n");
}

void ConsoleUI::editTransaction()
{
    std::uint64_t transactionId;
    if (!readId("Transaction ID: ", transactionId)) return;
    TransactionType type;
    if (!readTransactionType(type)) return;
    std::string description;
    std::cout << "New description: ";
    std::getline(std::cin, description);
    std::int64_t amount;
    if (!readAmount(amount)) return;

    std::cout << (m_transactionService.editTransaction(
                      m_currentUserId, transactionId, type, description, amount)
                      ? "Transaction updated successfully.\n\n"
                      : "Could not update transaction.\n\n");
}

void ConsoleUI::deleteTransaction()
{
    std::uint64_t transactionId;
    if (!readId("Transaction ID: ", transactionId)) return;
    std::cout << (m_transactionService.deleteTransaction(m_currentUserId, transactionId)
                      ? "Transaction deleted successfully.\n\n"
                      : "Could not delete transaction.\n\n");
}

void ConsoleUI::printCellBalance()
{
    const std::uint64_t cellId = m_currentCellId;
    const auto balance = m_transactionService.getCellBalance(m_currentUserId, cellId);
    if (!balance)
    {
        std::cout << "Cell not found or access denied.\n" << std::endl;
        return;
    }
    const auto transactions = m_transactionService.getTransactionsForCell(m_currentUserId, cellId);
    std::int64_t income = 0;
    std::int64_t expenses = 0;
    if (transactions)
    {
        for (const Transaction& transaction : *transactions)
        {
            if (transaction.getType() == TransactionType::INCOME)
                income += transaction.getAmountInMinorUnits();
            else expenses += transaction.getAmountInMinorUnits();
        }
    }
    std::cout << "Total income: " << formatMoney(income)
              << "\nTotal expenses: " << formatMoney(expenses)
              << "\nCurrent balance: " << formatMoney(*balance) << '\n';

    std::string month;
    std::cout << "Month summary YYYY-MM (blank to skip): ";
    std::getline(std::cin, month);
    if (!month.empty())
    {
        const auto monthly = m_transactionService.getTransactionsForCell(
            m_currentUserId, cellId, month + "-01", month + "-31");
        std::int64_t monthlyIncome = 0;
        std::int64_t monthlyExpenses = 0;
        if (monthly)
        {
            for (const Transaction& transaction : *monthly)
            {
                if (transaction.getType() == TransactionType::INCOME)
                    monthlyIncome += transaction.getAmountInMinorUnits();
                else monthlyExpenses += transaction.getAmountInMinorUnits();
            }
        }
        std::cout << "Monthly income: " << formatMoney(monthlyIncome)
                  << "\nMonthly expenses: " << formatMoney(monthlyExpenses)
                  << "\nMonthly net: " << formatMoney(monthlyIncome - monthlyExpenses) << '\n';
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
    std::cout << "0. Exit" << std::endl;
    std::cout << "==============" << std::endl;
}

void ConsoleUI::displayUserActionMenu() const
{
    std::cout << "==============" << std::endl;
    std::cout << "User Actions" << std::endl;
    std::cout << "==============" << std::endl;
    std::cout << "1. Create Cell" << std::endl;
    std::cout << "2. View My Cells" << std::endl;
    std::cout << "3. Select Cell" << std::endl;
    std::cout << "4. Logout" << std::endl;
    std::cout << "==============" << std::endl;
}

void ConsoleUI::displayCellActionMenu() const
{
    std::cout << "========== Cell " << m_currentCellId << " ==========\n"
              << "1. View Transactions\n2. Add Transaction\n3. Edit Transaction\n"
              << "4. Delete Transaction\n5. View Balance and Summary\n"
              << "6. Manage Members\n7. Edit Cell\n8. Delete Cell\n9. Back\n"
              << "============================\n";
}

void ConsoleUI::selectCell()
{
    std::uint64_t cellId;
    if (!readId("Cell ID: ", cellId)) return;
    if (!m_cellService.getCellForUser(m_currentUserId, cellId))
    {
        std::cout << "Cell not found or access denied.\n" << std::endl;
        return;
    }
    m_currentCellId = cellId;
}

void ConsoleUI::manageMembers()
{
    std::cout << "1. View Members\n2. Add Member\n3. Change Role\n4. Remove Member\n";
    int choice;
    if (!readChoice(choice)) return;
    if (choice == 1) printCellMembers();
    else if (choice == 2) addCellMember();
    else if (choice == 3) changeCellMemberRole();
    else if (choice == 4) removeCellMember();
    else std::cout << "Invalid member action.\n" << std::endl;
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

bool ConsoleUI::readPassword(const std::string& prompt, std::string& password) const
{
    std::cout << prompt << std::flush;
    termios original{};
    const bool terminal = isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &original) == 0;
    if (terminal)
    {
        termios hidden = original;
        hidden.c_lflag &= static_cast<tcflag_t>(~ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &hidden);
    }
    const bool read = static_cast<bool>(std::getline(std::cin, password));
    if (terminal)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &original);
        std::cout << '\n';
    }
    return read;
}

bool ConsoleUI::readId(const std::string& prompt, std::uint64_t& value) const
{
    std::cout << prompt;
    std::string input;
    if (!std::getline(std::cin, input)) return false;
    input = StringUtils::trim(input);
    const char* begin = input.data();
    const char* end = begin + input.size();
    const auto result = std::from_chars(begin, end, value);
    if (input.empty() || result.ec != std::errc{} || result.ptr != end || value == 0)
    {
        std::cout << "Please enter a valid positive ID.\n" << std::endl;
        return false;
    }
    return true;
}

bool ConsoleUI::readRole(CellRole& role) const
{
    std::uint64_t choice;
    if (!readId("Role (1 = MEMBER, 2 = GUEST): ", choice)) return false;
    if (choice == 1) role = CellRole::MEMBER;
    else if (choice == 2) role = CellRole::GUEST;
    else
    {
        std::cout << "Invalid role.\n" << std::endl;
        return false;
    }
    return true;
}

bool ConsoleUI::readTransactionType(TransactionType& type) const
{
    std::uint64_t choice;
    if (!readId("Type (1 = INCOME, 2 = EXPENSE): ", choice)) return false;
    if (choice == 1) type = TransactionType::INCOME;
    else if (choice == 2) type = TransactionType::EXPENSE;
    else
    {
        std::cout << "Invalid transaction type.\n" << std::endl;
        return false;
    }
    return true;
}

bool ConsoleUI::readAmount(std::int64_t& amountInMinorUnits) const
{
    std::cout << "Amount (for example 123.45): ";
    std::string input;
    if (!std::getline(std::cin, input)) return false;
    input = StringUtils::trim(input);

    const std::size_t decimalPoint = input.find('.');
    if (input.empty() || decimalPoint == 0 ||
        input.find('.', decimalPoint == std::string::npos ? 0 : decimalPoint + 1) != std::string::npos)
    {
        std::cout << "Invalid amount.\n" << std::endl;
        return false;
    }

    std::string whole = decimalPoint == std::string::npos ? input : input.substr(0, decimalPoint);
    std::string fraction = decimalPoint == std::string::npos ? "" : input.substr(decimalPoint + 1);
    if (fraction.length() > 2 ||
        !std::all_of(whole.begin(), whole.end(), [](unsigned char c) { return std::isdigit(c); }) ||
        !std::all_of(fraction.begin(), fraction.end(), [](unsigned char c) { return std::isdigit(c); }))
    {
        std::cout << "Invalid amount.\n" << std::endl;
        return false;
    }

    while (fraction.length() < 2) fraction.push_back('0');
    const std::string minorText = whole + fraction;
    std::uint64_t unsignedAmount = 0;
    const auto parsed = std::from_chars(
        minorText.data(), minorText.data() + minorText.size(), unsignedAmount);
    if (parsed.ec != std::errc{} || parsed.ptr != minorText.data() + minorText.size() ||
        unsignedAmount == 0 ||
        unsignedAmount > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
    {
        std::cout << "Amount must be positive and within range.\n" << std::endl;
        return false;
    }

    amountInMinorUnits = static_cast<std::int64_t>(unsignedAmount);
    return true;
}

std::string ConsoleUI::formatMoney(std::int64_t amountInMinorUnits) const
{
    const bool negative = amountInMinorUnits < 0;
    const std::uint64_t magnitude = negative
        ? static_cast<std::uint64_t>(-(amountInMinorUnits + 1)) + 1
        : static_cast<std::uint64_t>(amountInMinorUnits);
    std::ostringstream output;
    if (negative) output << '-';
    output << magnitude / 100 << '.' << std::setw(2) << std::setfill('0')
           << magnitude % 100 << " ILS";
    return output.str();
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

bool ConsoleUI::validateCellName(const std::string& cellName) const
{
    if (!m_cellService.isCellNameValid(cellName))
    {
        std::cout << "Cell name must be between " << CellService::MIN_CELL_NAME_LENGTH << " and "
            << CellService::MAX_CELL_NAME_LENGTH << " characters.\n" << std::endl;
        return false;
    }

    return true;
}

bool ConsoleUI::validateCellDescription(const std::string& cellDescription) const
{
    if (!m_cellService.isDescriptionValid(cellDescription))
    {
        std::cout << "Cell description must be between " << CellService::MIN_DESCRIPTION_LENGTH << " and "
            << CellService::MAX_DESCRIPTION_LENGTH << " characters.\n" << std::endl;
        return false;
    }

    return true;
}
