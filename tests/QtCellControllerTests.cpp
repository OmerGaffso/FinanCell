#include <cstdlib>
#include <iostream>
#include <string>

#include <QVariantMap>

#include "application/CellService.h"
#include "application/CategoryService.h"
#include "application/TransactionService.h"
#include "application/MonthlyReportService.h"
#include "storage/sqlite/SQLiteCategoryRepository.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteTransactionRepository.h"
#include "storage/sqlite/SQLiteUserRepository.h"
#include "ui/qt/controllers/CellController.h"
#include "ui/qt/controllers/CategoryController.h"
#include "ui/qt/controllers/MemberController.h"
#include "ui/qt/controllers/TransactionController.h"
#include "ui/qt/controllers/ReportController.h"
#include "ui/qt/session/SessionState.h"

namespace
{
/** @brief Stops the test executable when a condition fails. @param condition Expected condition. @param message Failure description. */
void require(bool condition, const std::string& message)
{
    if (condition) return;
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
}
}

/** @brief Exercises Qt cell-controller flows against an in-memory SQLite database. */
int main()
{
    SQLiteDatabase database(":memory:");
    SQLiteMigrations::apply(database);
    SQLiteUserRepository userRepository(database);
    SQLiteCellRepository cellRepository(database);
    SQLiteCategoryRepository categoryRepository(database);
    SQLiteTransactionRepository transactionRepository(database);
    CellService cellService(cellRepository, userRepository);
    CategoryService categoryService(categoryRepository, cellRepository);
    TransactionService transactionService(
        transactionRepository, cellRepository, categoryRepository);
    MonthlyReportService reportService(transactionRepository, cellRepository);

    require(
        userRepository.insertUser("owner", "Cell Owner", "unused-test-hash"),
        "insert owner fixture");
    require(
        userRepository.insertUser("other", "Other User", "unused-test-hash"),
        "insert second-user fixture");
    const auto owner = userRepository.findUserByUsername("owner");
    const auto other = userRepository.findUserByUsername("other");
    require(owner && other, "load user fixtures");

    SessionState session;
    CellController controller(cellService, transactionService, session);
    MemberController memberController(cellService, session);
    CategoryController categoryController(categoryService, cellService, session);
    TransactionController transactionController(transactionService, cellService, session);
    ReportController reportController(reportService, cellService, session);
    require(!controller.loadCells(), "logged-out users cannot load cells");

    session.setUser(
        owner->getUserId(),
        QStringLiteral("owner"),
        QStringLiteral("Cell Owner"));
    require(controller.loadCells() && controller.cells().isEmpty(),
            "new owner starts with an empty cell list");
    require(!controller.createCell(QStringLiteral("x"), QString()),
            "invalid cell name is rejected");
    require(controller.createCell(
                QStringLiteral("Home Budget"),
                QStringLiteral("Shared household costs")),
            "valid financial cell is created");
    require(controller.cells().size() == 1,
            "created financial cell appears in the controller list");

    const QVariantMap createdCell = controller.cells().front().toMap();
    const qulonglong cellId = createdCell.value(QStringLiteral("cellId")).toULongLong();
    require(createdCell.value(QStringLiteral("balanceText")).toString() ==
                QStringLiteral("0.00 ILS"),
            "new financial cell exposes a zero balance");

    const auto categories = categoryRepository.findCategoriesByCellId(cellId);
    require(!categories.empty(), "new cell receives its default category");
    require(transactionService.addTransaction(
                owner->getUserId(),
                cellId,
                TransactionType::INCOME,
                "Salary",
                12345,
                "2026-08-01",
                categories.front().getCategoryId()).has_value(),
            "add income balance fixture");
    require(controller.loadCells(), "reload cells after adding income");
    require(controller.cells().front().toMap()
                .value(QStringLiteral("balanceText")).toString() ==
                QStringLiteral("123.45 ILS"),
            "positive current balance is formatted for the GUI");
    require(transactionService.addTransaction(
                owner->getUserId(),
                cellId,
                TransactionType::EXPENSE,
                "Rent",
                20000,
                "2026-08-02",
                categories.front().getCategoryId()).has_value(),
            "add expense balance fixture");
    require(controller.loadCells(), "reload cells after adding expense");
    require(controller.cells().front().toMap()
                .value(QStringLiteral("balanceText")).toString() ==
                QStringLiteral("-76.55 ILS"),
            "negative current balance includes a minus sign");
    require(cellId != 0 && controller.selectCell(cellId),
            "owner can select the created financial cell");
    require(controller.hasSelectedCell() &&
                controller.selectedCell().value(QStringLiteral("name")).toString() ==
                    QStringLiteral("Home Budget"),
            "selected cell exposes its public details");

    session.setUser(
        other->getUserId(),
        QStringLiteral("other"),
        QStringLiteral("Other User"));
    require(controller.cells().isEmpty() && !controller.hasSelectedCell(),
            "identity changes clear cached cells and selection");
    require(controller.loadCells() && controller.cells().isEmpty(),
            "unrelated user cannot list the owner's cell");
    require(!controller.selectCell(cellId),
            "unrelated user cannot select the owner's cell");

    session.setUser(
        owner->getUserId(),
        QStringLiteral("owner"),
        QStringLiteral("Cell Owner"));
    require(memberController.loadMembers(cellId) &&
                memberController.members().size() == 1 &&
                memberController.canManage(),
            "owner loads and can manage public member summaries");
    require(memberController.addMember(cellId, other->getUserId(), "MEMBER") &&
                memberController.members().size() == 2,
            "owner adds a selected registered user");
    require(memberController.updateMemberRole(
                cellId, other->getUserId(), "GUEST"),
            "owner changes a member role");
    require(memberController.removeMember(cellId, other->getUserId()) &&
                memberController.members().size() == 1,
            "owner removes a cell member");
    require(categoryController.loadCategories(cellId) &&
                categoryController.categories().size() == 1 &&
                categoryController.canCreate(),
            "owner loads the default category with creation access");
    require(categoryController.createCategory(cellId, "Food") &&
                categoryController.categories().size() == 2,
            "owner creates a category through the Qt controller");
    require(!categoryController.createCategory(cellId, "food"),
            "duplicate category is rejected case-insensitively");
    require(transactionController.loadTransactions(cellId) &&
                transactionController.transactions().size() == 2 &&
                transactionController.canWrite(),
            "owner loads transactions with write access");
    require(!transactionController.addTransaction(
                cellId, "INCOME", "Invalid", "1.234", "2026-08-03",
                categories.front().getCategoryId()),
            "transaction controller rejects excess decimal precision");
    const auto food = categoryRepository.findCategoryByName(cellId, "Food");
    require(food && transactionController.addTransaction(
                cellId, "INCOME", "Refund", "10.50", "2026-08-03",
                food->getCategoryId()),
            "transaction controller adds exact income with a category");
    const QVariantMap editableTransaction =
        transactionController.transactions().front().toMap();
    require(transactionController.selectTransaction(
                editableTransaction.value("transactionId").toULongLong()),
            "owner selects a transaction for editing");
    require(transactionController.updateTransaction(
                cellId, "INCOME", "Updated salary", "125.00", "2026-08-01",
                categories.front().getCategoryId()),
            "owner edits a transaction through the controller");
    const qulonglong deleteId = transactionController.transactions().back().toMap()
        .value("transactionId").toULongLong();
    require(transactionController.deleteTransaction(cellId, deleteId),
            "owner deletes a transaction through the controller");
    require(!reportController.generateReport(cellId, "2026-13"),
            "report controller rejects an invalid month");
    require(reportController.generateReport(cellId, "2026-08") &&
                reportController.totalIncomeText() == "125.00 ILS" &&
                reportController.totalExpensesText() == "200.00 ILS" &&
                reportController.balanceText() == "-75.00 ILS",
            "report controller exposes monthly totals and balance");

    std::cout << "Qt cell controller tests passed.\n";
    return 0;
}
