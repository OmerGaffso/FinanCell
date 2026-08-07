#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#include <QVariantMap>

#include "application/CategoryService.h"
#include "application/CellService.h"
#include "application/MonthlyReportService.h"
#include "application/TransactionService.h"
#include "application/UserService.h"
#include "security/PasswordHasher.h"
#include "storage/sqlite/SQLiteCategoryRepository.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteTransactionRepository.h"
#include "storage/sqlite/SQLiteUserRepository.h"
#include "ui/qt/controllers/CategoryController.h"
#include "ui/qt/controllers/CellController.h"
#include "ui/qt/controllers/MemberController.h"
#include "ui/qt/controllers/ReportController.h"
#include "ui/qt/controllers/TransactionController.h"
#include "ui/qt/controllers/UserController.h"
#include "ui/qt/session/SessionState.h"

namespace
{
/** @brief Stops the acceptance executable when an expected user flow fails. */
void require(bool condition, const std::string& message)
{
    if (condition) return;
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
}

/** @brief Owns an isolated directory removed after the acceptance pass. */
class TemporaryDirectory final
{
public:
    /** @brief Creates a uniquely named directory under the system temporary path. */
    TemporaryDirectory()
    {
        const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path = std::filesystem::temp_directory_path() /
                 ("financell-mvp-acceptance-" + std::to_string(suffix));
        std::filesystem::create_directories(m_path);
    }

    /** @brief Removes all acceptance data without touching application data. */
    ~TemporaryDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    /** @brief Returns the temporary database directory. */
    const std::filesystem::path& path() const { return m_path; }

private:
    std::filesystem::path m_path;
};

/** @brief Finds a controller transaction by its description. */
QVariantMap transactionNamed(const QVariantList& transactions, const QString& description)
{
    for (const QVariant& value : transactions)
    {
        const QVariantMap transaction = value.toMap();
        if (transaction.value(QStringLiteral("description")).toString() == description)
            return transaction;
    }
    return {};
}

/** @brief Runs the complete MVP through Qt-facing controllers and a reopened SQLite file. */
void runAcceptancePass(const std::filesystem::path& databasePath)
{
    qulonglong cellId = 0;
    qulonglong memberId = 0;
    qulonglong guestId = 0;

    {
        SQLiteDatabase database(databasePath.string());
        SQLiteMigrations::apply(database);
        SQLiteUserRepository users(database);
        SQLiteCellRepository cells(database);
        SQLiteCategoryRepository categories(database);
        SQLiteTransactionRepository transactions(database);
        SodiumPasswordHasher passwordHasher;
        UserService userService(users, passwordHasher);
        CellService cellService(cells, users);
        CategoryService categoryService(categories, cells);
        TransactionService transactionService(transactions, cells, categories);
        MonthlyReportService reportService(transactions, cells, categories);
        SessionState session;
        UserController user(userService, session);
        CellController cell(cellService, transactionService, session);
        MemberController member(cellService, session);
        CategoryController category(categoryService, cellService, session);
        TransactionController transaction(transactionService, cellService, session);
        ReportController report(reportService, cellService, session);

        require(user.registerUser("owner", "Cell Owner", "secret1"),
                "register owner through GUI controller");
        require(user.registerUser("member", "Cell Member", "secret2"),
                "register member through GUI controller");
        require(user.registerUser("guest", "Cell Guest", "secret3"),
                "register guest through GUI controller");
        require(user.registerUser("newcomer", "New Cell User", "secret4"),
                "register a user for member-managed access");
        require(!user.registerUser("OWNER", "Duplicate Owner", "secret4"),
                "reject duplicate normalized username");
        require(!user.login("owner", "wrong-password"), "reject invalid login");
        require(user.login("OWNER", "secret1") && user.loggedIn(),
                "log in with normalized username");

        memberId = users.findUserByUsername("member")->getUserId();
        guestId = users.findUserByUsername("guest")->getUserId();
        require(cell.createCell("Household", "Shared monthly expenses"),
                "create financial cell");
        require(cell.cells().size() == 1, "new cell appears in list");
        cellId = cell.cells().front().toMap().value("cellId").toULongLong();
        require(cell.selectCell(cellId), "open new financial cell");
        require(member.addMember(cellId, memberId, "MEMBER"), "add member");
        require(member.addMember(cellId, guestId, "GUEST"), "add guest");
        require(member.loadMembers(cellId) && member.members().size() == 3 &&
                    member.canManage(),
                "owner sees and manages all memberships");

        require(category.loadCategories(cellId) && category.categories().size() == 1,
                "load default category");
        require(category.createCategory(cellId, "Salary"), "create income category");
        require(category.createCategory(cellId, "Food"), "create expense category");
        require(!category.createCategory(cellId, "food"),
                "reject duplicate category case-insensitively");
        const auto salary = categories.findCategoryByName(cellId, "Salary");
        const auto food = categories.findCategoryByName(cellId, "Food");
        require(salary && food, "load created categories");
        require(category.setMonthlyBudget(cellId, food->getCategoryId(), "600.00"),
                "set a monthly category budget");

        require(transaction.addTransaction(
                    cellId, "INCOME", "August salary", "3000.00", "2026-08-01",
                    salary->getCategoryId()),
                "add income");
        require(transaction.addTransaction(
                    cellId, "EXPENSE", "Groceries", "250.00", "2026-08-05",
                    food->getCategoryId()),
                "add expense");
        require(transaction.addTransaction(
                    cellId, "EXPENSE", "Delete me", "10.00", "2026-08-06",
                    food->getCategoryId()),
                "add transaction for deletion");
        require(transaction.loadTransactions(cellId, "2026-08-05", "2026-08-05") &&
                    transaction.transactions().size() == 1,
                "filter transactions by inclusive date range");
        require(transaction.loadTransactions(cellId), "reload all transactions");
        const QVariantMap groceries = transactionNamed(
            transaction.transactions(), QStringLiteral("Groceries"));
        require(!groceries.isEmpty() && transaction.selectTransaction(
                    groceries.value("transactionId").toULongLong()),
                "select expense for editing");
        require(transaction.updateTransaction(
                    cellId, "EXPENSE", "Groceries updated", "200.00", "2026-08-05",
                    food->getCategoryId()),
                "edit expense");
        const QVariantMap deletion = transactionNamed(
            transaction.transactions(), QStringLiteral("Delete me"));
        require(!deletion.isEmpty() && transaction.deleteTransaction(
                    cellId, deletion.value("transactionId").toULongLong()),
                "delete expense");
        require(report.generateReport(cellId, "2026-08") &&
                    report.totalIncomeText() == QStringLiteral("3000.00 ILS") &&
                    report.totalExpensesText() == QStringLiteral("200.00 ILS") &&
                    report.balanceText() == QStringLiteral("2800.00 ILS") &&
                    report.categoryLines().size() == 2,
                "generate monthly totals and category breakdown");
        bool budgetProgressPresented = false;
        for (const QVariant& value : report.categoryLines())
        {
            const QVariantMap line = value.toMap();
            if (line.value(QStringLiteral("categoryName")).toString() ==
                    QStringLiteral("Food") &&
                line.value(QStringLiteral("budgetText")).toString() ==
                    QStringLiteral("600.00 ILS") &&
                line.value(QStringLiteral("remainingBudgetText")).toString() ==
                    QStringLiteral("400.00 ILS") &&
                !line.value(QStringLiteral("overBudget")).toBool())
                budgetProgressPresented = true;
        }
        require(budgetProgressPresented,
                "monthly report presents remaining category budget");
        require(cell.updateSelectedCell("Household 2026", "Updated shared expenses"),
                "owner edits cell details");

        user.logout();
        require(user.login("member", "secret2") && cell.loadCells() &&
                    cell.selectCell(cellId),
                "member logs in and opens shared cell");
        const auto newcomerId = users.findUserByUsername("newcomer")->getUserId();
        require(member.loadMembers(cellId) && !member.canManage() &&
                    member.canAddMembers(),
                "member can add users without managing existing roles");
        require(member.addMember(cellId, newcomerId, "GUEST"),
                "member adds a registered user as guest");
        require(!member.updateMemberRole(cellId, newcomerId, "MANAGER"),
                "member cannot promote a user to manager");
        require(category.loadCategories(cellId) && category.canCreate(),
                "member can create categories");
        require(transaction.loadTransactions(cellId) && transaction.canWrite(),
                "member can write transactions");
        require(!cell.updateSelectedCell("Denied", "Denied"),
                "member cannot edit cell details");

        user.logout();
        require(user.login("guest", "secret3") && cell.loadCells() &&
                    cell.selectCell(cellId),
                "guest logs in and opens shared cell");
        require(member.loadMembers(cellId) && !member.canManage() &&
                    !member.canAddMembers(),
                "guest cannot change membership");
        require(category.loadCategories(cellId) && !category.canCreate(),
                "guest cannot create categories");
        require(transaction.loadTransactions(cellId) && !transaction.canWrite(),
                "guest has read-only transaction access");
        require(!transaction.addTransaction(
                    cellId, "EXPENSE", "Denied", "1.00", "2026-08-07",
                    food->getCategoryId()),
                "guest transaction creation is rejected");
    }

    {
        SQLiteDatabase database(databasePath.string());
        SQLiteMigrations::apply(database);
        SQLiteUserRepository users(database);
        SQLiteCellRepository cells(database);
        SQLiteCategoryRepository categories(database);
        SQLiteTransactionRepository transactions(database);
        SodiumPasswordHasher passwordHasher;
        UserService userService(users, passwordHasher);
        CellService cellService(cells, users);
        CategoryService categoryService(categories, cells);
        TransactionService transactionService(transactions, cells, categories);
        MonthlyReportService reportService(transactions, cells, categories);
        SessionState session;
        UserController user(userService, session);
        CellController cell(cellService, transactionService, session);
        MemberController member(cellService, session);
        CategoryController category(categoryService, cellService, session);
        TransactionController transaction(transactionService, cellService, session);
        ReportController report(reportService, cellService, session);

        require(user.login("owner", "secret1"), "owner login survives restart");
        require(cell.loadCells() && cell.cells().size() == 1 && cell.selectCell(cellId),
                "edited cell survives restart");
        require(cell.selectedCell().value("name").toString() ==
                    QStringLiteral("Household 2026"),
                "cell edits persist after restart");
        require(member.loadMembers(cellId) && member.members().size() == 4,
                "members persist after restart");
        require(category.loadCategories(cellId) && category.categories().size() == 3,
                "categories persist after restart");
        const auto persistedFood = categories.findCategoryByName(cellId, "Food");
        require(persistedFood && persistedFood->getMonthlyBudgetInMinorUnits() == 60000,
                "category budget persists after restart");
        require(transaction.loadTransactions(cellId) && transaction.transactions().size() == 2,
                "transaction edits and deletion persist after restart");
        require(report.generateReport(cellId, "2026-08") &&
                    report.balanceText() == QStringLiteral("2800.00 ILS"),
                "monthly report persists after restart");
        require(cell.deleteSelectedCell() && cell.cells().isEmpty(),
                "owner deletes the cell after restart");
        require(!cells.findCellById(cellId), "cell deletion persists");
    }
}
}

/** @brief Runs the isolated file-backed MVP acceptance pass. */
int main()
{
    TemporaryDirectory directory;
    runAcceptancePass(directory.path() / "acceptance.db");
    std::cout << "GUI MVP acceptance pass completed successfully.\n";
    return 0;
}
