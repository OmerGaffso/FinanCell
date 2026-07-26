#include "ui/console/ConsoleUI.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

#include "utils/StringUtils.h"

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
                    createCell();
                    break;
                case 2:
                    printCells();
                    break;
                case 3:
                    printCellMembers();
                    break;
                case 4:
                    addCellMember();
                    break;
                case 5:
                    changeCellMemberRole();
                    break;
                case 6:
                    removeCellMember();
                    break;
                case 7:
                    editCell();
                    break;
                case 8:
                    deleteCell();
                    break;
                case 9:
                    printTransactions();
                    break;
                case 10:
                    addTransaction();
                    break;
                case 11:
                    editTransaction();
                    break;
                case 12:
                    deleteTransaction();
                    break;
                case 13:
                    printCellBalance();
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
    std::uint64_t cellId;
    if (!readId("Cell ID: ", cellId)) return;

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
        std::cout << "User ID: " << member.userId << ", Role: " << role << '\n';
    }
    std::cout << std::endl;
}

void ConsoleUI::addCellMember()
{
    std::uint64_t cellId;
    if (!readId("Cell ID: ", cellId)) return;

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
    if (m_cellService.addMemberToCell(
            m_currentUserId, cellId, user->getUserId(), role))
    {
        std::cout << "Member added successfully.\n" << std::endl;
    }
    else
    {
        std::cout << "Could not add member. Check ownership and existing membership.\n" << std::endl;
    }
}

void ConsoleUI::changeCellMemberRole()
{
    std::uint64_t cellId;
    std::uint64_t userId;
    if (!readId("Cell ID: ", cellId) || !readId("Member user ID: ", userId)) return;
    CellRole role;
    if (!readRole(role)) return;

    std::cout << (m_cellService.updateMemberRole(
                      m_currentUserId, cellId, userId, role)
                      ? "Member role updated successfully.\n\n"
                      : "Could not update member role.\n\n");
}

void ConsoleUI::removeCellMember()
{
    std::uint64_t cellId;
    std::uint64_t userId;
    if (!readId("Cell ID: ", cellId) || !readId("Member user ID: ", userId)) return;
    std::cout << (m_cellService.removeMemberFromCell(
                      m_currentUserId, cellId, userId)
                      ? "Member removed successfully.\n\n"
                      : "Could not remove member.\n\n");
}

void ConsoleUI::editCell()
{
    std::uint64_t cellId;
    if (!readId("Cell ID: ", cellId)) return;
    std::string name;
    std::string description;
    std::cout << "New cell name: ";
    std::getline(std::cin, name);
    std::cout << "New description: ";
    std::getline(std::cin, description);
    std::cout << (m_cellService.updateCell(
                      m_currentUserId, cellId, name, description)
                      ? "Cell updated successfully.\n\n"
                      : "Could not update cell.\n\n");
}

void ConsoleUI::deleteCell()
{
    std::uint64_t cellId;
    if (!readId("Cell ID: ", cellId)) return;
    std::string confirmation;
    std::cout << "Type DELETE to permanently delete the cell: ";
    std::getline(std::cin, confirmation);
    if (confirmation != "DELETE")
    {
        std::cout << "Deletion cancelled.\n" << std::endl;
        return;
    }
    std::cout << (m_cellService.deleteCell(m_currentUserId, cellId)
                      ? "Cell deleted successfully.\n\n"
                      : "Could not delete cell.\n\n");
}

void ConsoleUI::printTransactions()
{
    std::uint64_t cellId;
    if (!readId("Cell ID: ", cellId)) return;
    const auto transactions =
        m_transactionService.getTransactionsForCell(m_currentUserId, cellId);
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
                  << ", Created by user: " << transaction.getUserId()
                  << ", Date: " << transaction.getOccurredAt() << '\n';
    }
    std::cout << std::endl;
}

void ConsoleUI::addTransaction()
{
    std::uint64_t cellId;
    if (!readId("Cell ID: ", cellId)) return;
    TransactionType type;
    if (!readTransactionType(type)) return;
    std::string description;
    std::cout << "Description: ";
    std::getline(std::cin, description);
    std::int64_t amount;
    if (!readAmount(amount)) return;

    const auto transaction = m_transactionService.addTransaction(
        m_currentUserId, cellId, type, description, amount);
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
    std::uint64_t cellId;
    if (!readId("Cell ID: ", cellId)) return;
    const auto balance = m_transactionService.getCellBalance(m_currentUserId, cellId);
    if (!balance)
    {
        std::cout << "Cell not found or access denied.\n" << std::endl;
        return;
    }
    std::cout << "Cell balance: " << formatMoney(*balance) << '\n' << std::endl;
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
    std::cout << "2. View My Cells" << std::endl;
    std::cout << "3. View Cell Members" << std::endl;
    std::cout << "4. Add Cell Member" << std::endl;
    std::cout << "5. Change Member Role" << std::endl;
    std::cout << "6. Remove Cell Member" << std::endl;
    std::cout << "7. Edit Cell" << std::endl;
    std::cout << "8. Delete Cell" << std::endl;
    std::cout << "9. View Transactions" << std::endl;
    std::cout << "10. Add Transaction" << std::endl;
    std::cout << "11. Edit Transaction" << std::endl;
    std::cout << "12. Delete Transaction" << std::endl;
    std::cout << "13. View Cell Balance" << std::endl;
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
