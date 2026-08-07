#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "application/CategoryService.h"
#include "application/CellService.h"
#include "application/MonthlyReportService.h"
#include "application/PersistenceError.h"
#include "application/TransactionService.h"
#include "application/UserService.h"
#include "security/PasswordHasher.h"
#include "storage/sqlite/SQLiteCategoryRepository.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteTransactionRepository.h"
#include "storage/sqlite/SQLiteUserRepository.h"
#include "ui/console/ConsoleUI.h"

namespace
{
void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}

class RedirectStandardStreams
{
public:
    RedirectStandardStreams(std::istream& input, std::ostream& output)
        : m_inputBuffer(std::cin.rdbuf(input.rdbuf())),
          m_outputBuffer(std::cout.rdbuf(output.rdbuf())),
          m_errorBuffer(std::cerr.rdbuf(output.rdbuf()))
    {
    }

    ~RedirectStandardStreams()
    {
        std::cin.rdbuf(m_inputBuffer);
        std::cout.rdbuf(m_outputBuffer);
        std::cerr.rdbuf(m_errorBuffer);
    }

private:
    std::streambuf* m_inputBuffer;
    std::streambuf* m_outputBuffer;
    std::streambuf* m_errorBuffer;
};

class ThrowingCellRepository final : public CellRepository
{
public:
    explicit ThrowingCellRepository(CellRepository& delegate) : m_delegate(delegate) {}

    std::optional<FinancialCell> insertCell(const FinancialCell& cell) override
    {
        return m_delegate.insertCell(cell);
    }
    std::optional<FinancialCell> findCellById(std::uint64_t cellId) const override
    {
        return m_delegate.findCellById(cellId);
    }
    bool updateCell(const FinancialCell& cell) override { return m_delegate.updateCell(cell); }
    bool deleteCell(std::uint64_t cellId) override { return m_delegate.deleteCell(cellId); }
    std::vector<FinancialCell> findCellsByCreatorId(std::uint64_t creatorId) const override
    {
        return m_delegate.findCellsByCreatorId(creatorId);
    }
    std::vector<FinancialCell> findAllCells() const override
    {
        return m_delegate.findAllCells();
    }
    bool insertMember(const CellMember& member) override { return m_delegate.insertMember(member); }
    std::optional<CellMember> findMember(
        std::uint64_t,
        std::uint64_t) const override
    {
        throw PersistenceError("simulated read failure");
    }
    std::vector<CellMember> findMembersByCellId(std::uint64_t cellId) const override
    {
        return m_delegate.findMembersByCellId(cellId);
    }
    std::vector<FinancialCell> findCellsByUserId(std::uint64_t userId) const override
    {
        return m_delegate.findCellsByUserId(userId);
    }
    bool updateMemberRole(
        std::uint64_t cellId,
        std::uint64_t userId,
        CellRole role) override
    {
        return m_delegate.updateMemberRole(cellId, userId, role);
    }
    bool deleteMember(std::uint64_t cellId, std::uint64_t userId) override
    {
        return m_delegate.deleteMember(cellId, userId);
    }

private:
    CellRepository& m_delegate;
};
}

int main()
{
    SQLiteDatabase database(":memory:");
    SQLiteMigrations::apply(database);
    SQLiteUserRepository users(database);
    SQLiteCellRepository cells(database);
    SQLiteCategoryRepository categories(database);
    SQLiteTransactionRepository transactions(database);
    SodiumPasswordHasher hasher;
    UserService userService(users, hasher);
    CellService cellService(cells, users);
    CategoryService categoryService(categories, cells);
    TransactionService transactionService(transactions, cells, categories);
    MonthlyReportService reportService(transactions, cells, categories);

    std::string username = "owner";
    std::string displayName = "Owner User";
    std::string password = "secret1";
    require(userService.createUser(username, displayName, password), "seed console user");
    const auto owner = users.findUserByUsername(username);
    require(cellService.createCell("Console Cell", owner->getUserId(), "UI flow"),
            "seed console cell");

    std::istringstream input(
        "2\nowner\nsecret1\n"
        "3\n1\n"
        "7\n2\nSalary\n"
        "7\n2\nHousing\n"
        "2\n1\nPay\n1000\n2\n2025-03-01\n"
        "2\n2\nRent\n400\n3\n2025-03-05\n"
        "5\n2025-03\n"
        "0\n4\n0\n");
    std::ostringstream output;
    {
        RedirectStandardStreams redirect(input, output);
        ConsoleUI ui{
            userService,
            cellService,
            categoryService,
            transactionService,
            reportService};
        ui.runApp();
    }

    const std::string text = output.str();
    require(text.find("Login successful") != std::string::npos, "console login works");
    require(text.find("Category created successfully") != std::string::npos,
            "console category creation works");
    require(text.find("Transaction added successfully") != std::string::npos,
            "console transaction creation works");
    require(text.find("Monthly report for 2025-03") != std::string::npos,
            "console report is displayed");
    require(text.find("Total income: 1000.00 ILS") != std::string::npos,
            "console report income is correct");
    require(text.find("Total expenses: 400.00 ILS") != std::string::npos,
            "console report expenses are correct");
    require(text.find("Monthly balance: 600.00 ILS") != std::string::npos,
            "console report balance is correct");
    require(text.find("Salary: income 1000.00 ILS") != std::string::npos &&
                text.find("Housing: income 0.00 ILS, expenses 400.00 ILS") !=
                    std::string::npos,
            "console report category breakdown is correct");
    require(transactions.findTransactionsByCellId(1).size() == 2,
            "console flow persisted both transactions");

    ThrowingCellRepository throwingCells(cells);
    CellService failingCellService(throwingCells, users);
    CategoryService failingCategoryService(categories, throwingCells);
    TransactionService failingTransactionService(transactions, throwingCells, categories);
    MonthlyReportService failingReportService(transactions, throwingCells, categories);
    std::istringstream failureInput("2\nowner\nsecret1\n3\n1\n4\n0\n");
    std::ostringstream failureOutput;
    {
        RedirectStandardStreams redirect(failureInput, failureOutput);
        ConsoleUI ui{
            userService,
            failingCellService,
            failingCategoryService,
            failingTransactionService,
            failingReportService};
        ui.runApp();
    }
    require(failureOutput.str().find("The database operation failed. Please try again.") !=
                std::string::npos,
            "console reports a recoverable persistence failure");
    require(failureOutput.str().find("Logging out") != std::string::npos,
            "console continues after a persistence failure");
}
