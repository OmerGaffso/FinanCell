#include <limits>
#include <stdexcept>
#include <string>

#include "application/CellService.h"
#include "application/CategoryService.h"
#include "application/MonthlyReportService.h"
#include "application/TransactionService.h"
#include "application/UserService.h"
#include "security/PasswordHasher.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteCategoryRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteTransactionRepository.h"
#include "storage/sqlite/SQLiteUserRepository.h"

namespace
{
void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}
}

int main()
{
    SQLiteDatabase database(":memory:");
    SQLiteMigrations::apply(database);
    SQLiteUserRepository users(database);
    SQLiteCellRepository cells(database);
    SQLiteCategoryRepository categories(database);
    SQLiteTransactionRepository transactions(database);
    CellService cellService(cells, users);
    CategoryService categoryService(categories, cells);
    TransactionService transactionService(transactions, cells, categories);
    MonthlyReportService reportService(transactions, cells);
    SodiumPasswordHasher passwordHasher;
    UserService userService(users, passwordHasher);

    std::string ownerUsername = "owner";
    std::string ownerName = "Cell Owner";
    std::string ownerPassword = "secret1";
    std::string memberUsername = "member";
    std::string memberName = "Cell Member";
    std::string memberPassword = "secret2";
    std::string guestUsername = "guest";
    std::string guestName = "Cell Guest";
    std::string guestPassword = "secret3";
    require(userService.createUser(ownerUsername, ownerName, ownerPassword), "create owner");
    require(userService.createUser(memberUsername, memberName, memberPassword), "create member");
    require(userService.createUser(guestUsername, guestName, guestPassword), "create guest");
    const auto owner = users.findUserByUsername("owner");
    const auto member = users.findUserByUsername("member");
    const auto guest = users.findUserByUsername("guest");
    require(owner && member && guest, "load users");
    require(userService.authenticateUser("owner", "secret1").has_value(),
            "hashed login succeeds");
    require(!userService.authenticateUser("owner", "wrong-password"),
            "invalid password rejected");
    std::string secureUsername = "secure";
    std::string secureName = "Secure User";
    std::string securePassword = "secret4";
    require(userService.createUser(secureUsername, secureName, securePassword),
            "hashed account creation");
    require(passwordHasher.isEncodedHash(
                users.findUserByUsername("secure")->getPasswordHash()),
            "new account stores Argon2id hash");
    require(userService.authenticateUser("SECURE", "secret4").has_value(),
            "new hashed account authenticates with normalized username");
    require(!userService.createUser(secureUsername, secureName, securePassword),
            "duplicate username rejected");
    const auto allUserSummaries = userService.searchUsers(owner->getUserId(), "");
    require(allUserSummaries && allUserSummaries->size() == 4,
            "authenticated user lists the registered-user directory");
    const auto usernameMatches = userService.searchUsers(owner->getUserId(), "SEC");
    require(usernameMatches && usernameMatches->size() == 1 &&
                usernameMatches->front().getUsername() == "secure",
            "user directory searches usernames case-insensitively");
    const auto displayNameMatches = userService.searchUsers(
        owner->getUserId(), "cell member");
    require(displayNameMatches && displayNameMatches->size() == 1 &&
                displayNameMatches->front().getUserId() == member->getUserId(),
            "user directory searches display names case-insensitively");
    require(!userService.searchUsers(9999, "") &&
                !userService.searchUsers(
                    owner->getUserId(), std::string(51, 'x')),
            "user directory rejects invalid actors and oversized queries");

    require(cellService.createCell(" Family Budget ", owner->getUserId(), " Shared expenses "),
            "create cell");
    const auto ownedCells = cellService.getCellsForUser(owner->getUserId());
    require(ownedCells.size() == 1, "owner can see new cell");
    const uint64_t cellId = ownedCells.front().getCellId();
    require(cells.findMember(cellId, owner->getUserId())->role == CellRole::OWNER,
            "owner membership created");
    require(categories.findCategoryByName(cellId, "General").has_value(),
            "default category created with cell");

    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, member->getUserId(), CellRole::MEMBER) == CellOperationResult::SUCCESS,
            "owner adds member");
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, guest->getUserId(), CellRole::GUEST) == CellOperationResult::SUCCESS,
            "owner adds guest");
    require(cellService.addMemberToCell(
                member->getUserId(), cellId, guest->getUserId(), CellRole::MEMBER) == CellOperationResult::NOT_AUTHORIZED,
            "member cannot manage membership");
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, member->getUserId(), CellRole::MEMBER) == CellOperationResult::ALREADY_MEMBER,
            "duplicate membership rejected");
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, 9999, CellRole::MEMBER) == CellOperationResult::USER_NOT_FOUND,
            "unknown member rejected");
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, users.findUserByUsername("secure")->getUserId(), CellRole::OWNER) == CellOperationResult::INVALID_ROLE,
            "second owner role rejected");
    require(cellService.getCellsForUser(member->getUserId()).size() == 1,
            "member can see joined cell");
    const auto memberSummaries = cellService.getCellMemberSummaries(
        owner->getUserId(), cellId);
    require(memberSummaries && memberSummaries->size() == 3,
            "cell members load as public identity summaries");
    require(memberSummaries->front().getUsername() == "owner" &&
                memberSummaries->front().getRole() == CellRole::OWNER,
            "member summary includes username and role without account secrets");
    require(!cellService.getCellMemberSummaries(
                users.findUserByUsername("secure")->getUserId(), cellId),
            "non-members cannot load member summaries");
    require(cellService.updateMemberRole(
                owner->getUserId(), cellId, guest->getUserId(), CellRole::MEMBER) == CellOperationResult::SUCCESS,
            "owner changes member role");
    require(cells.findMember(cellId, guest->getUserId())->role == CellRole::MEMBER,
            "changed role persisted");
    require(cellService.updateMemberRole(
                owner->getUserId(), cellId, guest->getUserId(), CellRole::GUEST) == CellOperationResult::SUCCESS,
            "owner restores guest role");

    require(categoryService.createCategory(
                owner->getUserId(), cellId, " Salary ") == CategoryOperationResult::SUCCESS,
            "owner creates category");
    require(categoryService.createCategory(
                member->getUserId(), cellId, "Food") == CategoryOperationResult::SUCCESS,
            "member creates category");
    require(categoryService.createCategory(
                owner->getUserId(), cellId, "Kid's Food") == CategoryOperationResult::SUCCESS,
            "prepared category insert accepts an apostrophe");
    require(categoryService.createCategory(
                owner->getUserId(), cellId, "salary") == CategoryOperationResult::ALREADY_EXISTS,
            "category names are unique case-insensitively");
    require(categoryService.createCategory(
                guest->getUserId(), cellId, "Denied") == CategoryOperationResult::NOT_AUTHORIZED,
            "guest cannot create categories");
    require(categoryService.getCategoriesForCell(guest->getUserId(), cellId)->size() == 4,
            "guest can list categories");
    const auto salaryCategory = categories.findCategoryByName(cellId, "Salary");
    const auto foodCategory = categories.findCategoryByName(cellId, "Food");
    require(salaryCategory && foodCategory, "load managed categories");
    require(categoryService.setMonthlyBudget(
                owner->getUserId(), cellId, foodCategory->getCategoryId(), 50000) ==
                CategoryOperationResult::SUCCESS,
            "owner sets category budget");
    require(categories.findCategoryById(foodCategory->getCategoryId())
                ->getMonthlyBudgetInMinorUnits() == 50000,
            "category budget persists");
    require(categoryService.setMonthlyBudget(
                member->getUserId(), cellId, foodCategory->getCategoryId(), 45000) ==
                CategoryOperationResult::SUCCESS,
            "member updates category budget");
    require(categoryService.setMonthlyBudget(
                guest->getUserId(), cellId, foodCategory->getCategoryId(), 10000) ==
                CategoryOperationResult::NOT_AUTHORIZED,
            "guest cannot update category budget");
    require(categoryService.setMonthlyBudget(
                owner->getUserId(), cellId, foodCategory->getCategoryId(), -1) ==
                CategoryOperationResult::INVALID_INPUT,
            "negative category budget rejected");

    const auto income = transactionService.addTransaction(
        owner->getUserId(), cellId, TransactionType::INCOME, "Salary", 10000,
        "2026-07-01", salaryCategory->getCategoryId());
    const auto expense = transactionService.addTransaction(
        member->getUserId(), cellId, TransactionType::EXPENSE, "Food", 2500,
        "2026-07-02", foodCategory->getCategoryId());
    require(income && expense, "owner and member add transactions");
    require(income->getCategoryName() == "Salary", "transaction category persisted");
    require(!transactionService.addTransaction(
                owner->getUserId(), cellId, TransactionType::EXPENSE, "Bad date", 100,
                "2026-02-29", foodCategory->getCategoryId()),
            "invalid calendar date rejected");
    require(TransactionService::isDateValid("2024-02-29"),
            "valid leap date accepted");
    require(!TransactionService::isDateValid("2024-00-01") &&
                !TransactionService::isDateValid("2024-01-00") &&
                !TransactionService::isDateValid("2024-aa-01"),
            "malformed calendar dates rejected");
    require(!transactionService.addTransaction(
                owner->getUserId(), cellId, TransactionType::EXPENSE, "Bad category", 100,
                "2026-07-01", 9999),
            "unknown category rejected");
    require(transactionService.getTransactionsForCell(
                owner->getUserId(), cellId, "2026-07-01", "2026-07-01")->size() == 1,
            "transactions filter by date");
    require(!transactionService.getTransactionsForCell(
                owner->getUserId(), cellId, "2026-07-01", ""),
            "incomplete date range rejected");
    require(!transactionService.getTransactionsForCell(
                owner->getUserId(), cellId, "2026-08-01", "2026-07-01"),
            "reversed date range rejected");
    require(!transactionService.getTransactionsForCell(
                users.findUserByUsername("secure")->getUserId(), cellId),
            "outsider cannot read transactions");
    require(!transactionService.addTransaction(
                guest->getUserId(), cellId, TransactionType::EXPENSE, "Denied", 100,
                "", foodCategory->getCategoryId()),
            "guest cannot add transaction");
    require(!transactionService.editTransaction(
                member->getUserId(), cellId, income->getTransactionId(),
                TransactionType::INCOME, "Changed", 50000, "", salaryCategory->getCategoryId()),
            "member cannot edit owner's transaction");
    require(transactionService.editTransaction(
                owner->getUserId(), cellId, expense->getTransactionId(),
                TransactionType::EXPENSE, "Groceries", 2000, "2026-07-02", foodCategory->getCategoryId()),
            "owner edits any transaction");
    require(transactionService.editTransaction(
                owner->getUserId(), cellId, income->getTransactionId(),
                TransactionType::INCOME, "Salary revised", 10000, "", 0),
            "blank edit fields retain date and category");
    require(transactions.findTransactionById(income->getTransactionId())->getCategoryName() == "Salary" &&
                transactions.findTransactionById(income->getTransactionId())->getOccurredAt() == "2026-07-01",
            "retained transaction fields remain unchanged");
    require(transactions.findTransactionById(expense->getTransactionId())->getCategoryName() == "Food",
            "transaction edit persists category");
    require(transactions.findTransactionById(expense->getTransactionId())->getOccurredAt() == "2026-07-02",
            "transaction edit persists date");
    require(transactionService.getCellBalance(owner->getUserId(), cellId) == 8000,
            "balance reflects transaction edit");
    require(transactionService.addTransaction(
                owner->getUserId(), cellId, TransactionType::EXPENSE, "Month end", 100,
                "2026-07-31", foodCategory->getCategoryId()) &&
                transactionService.addTransaction(
                    owner->getUserId(), cellId, TransactionType::INCOME, "Previous month", 777,
                    "2026-06-30", salaryCategory->getCategoryId()) &&
                transactionService.addTransaction(
                    owner->getUserId(), cellId, TransactionType::EXPENSE, "Next month", 888,
                    "2026-08-01", foodCategory->getCategoryId()),
            "create transactions around monthly report boundaries");
    const auto report = reportService.generate(owner->getUserId(), cellId, "2026-07");
    require(report && report->totalIncomeInMinorUnits == 10000 &&
                report->totalExpensesInMinorUnits == 2100 &&
                report->balanceInMinorUnits == 7900 && report->categories.size() == 2,
            "monthly report contains totals, category breakdown, and exact month boundaries");
    const auto emptyReport = reportService.generate(owner->getUserId(), cellId, "2024-02");
    require(emptyReport && emptyReport->totalIncomeInMinorUnits == 0 &&
                emptyReport->totalExpensesInMinorUnits == 0 &&
                emptyReport->balanceInMinorUnits == 0 && emptyReport->categories.empty(),
            "empty leap-month report contains zero totals");
    require(!reportService.generate(owner->getUserId(), cellId, "2026-13") &&
                !reportService.generate(
                    users.findUserByUsername("secure")->getUserId(), cellId, "2026-07"),
            "invalid and unauthorized reports are rejected");

    require(cellService.createCell("Travel Budget", owner->getUserId(), "Trips"),
            "create second cell");
    const uint64_t secondCellId = cellService.getCellsForUser(owner->getUserId()).back().getCellId();
    require(categoryService.setMonthlyBudget(
                owner->getUserId(), secondCellId, foodCategory->getCategoryId(), 10000) ==
                CategoryOperationResult::CATEGORY_NOT_FOUND,
            "category budget cannot cross cells");
    require(categoryService.createCategory(
                owner->getUserId(), secondCellId, "Travel") == CategoryOperationResult::SUCCESS,
            "create category in second cell");
    const auto travelCategory = categories.findCategoryByName(secondCellId, "Travel");
    const auto secondCellTransaction = transactionService.addTransaction(
        owner->getUserId(), secondCellId, TransactionType::EXPENSE, "Flight", 5000,
        "2026-08-01", travelCategory->getCategoryId());
    require(secondCellTransaction.has_value(), "create transaction in second cell");
    require(transactionService.addTransaction(
                owner->getUserId(), secondCellId, TransactionType::INCOME, "Large one",
                std::numeric_limits<std::int64_t>::max(), "2027-01-01",
                travelCategory->getCategoryId()) &&
                transactionService.addTransaction(
                    owner->getUserId(), secondCellId, TransactionType::INCOME, "Large two",
                    std::numeric_limits<std::int64_t>::max(), "2027-01-02",
                    travelCategory->getCategoryId()),
            "store individually valid large transactions");
    bool reportOverflowDetected = false;
    try
    {
        static_cast<void>(reportService.generate(
            owner->getUserId(), secondCellId, "2027-01"));
    }
    catch (const std::overflow_error&)
    {
        reportOverflowDetected = true;
    }
    require(reportOverflowDetected, "monthly report detects integer overflow");
    require(!transactionService.editTransaction(
                owner->getUserId(), cellId, secondCellTransaction->getTransactionId(),
                TransactionType::EXPENSE, "Wrong cell", 1, "", travelCategory->getCategoryId()),
            "transaction edit is scoped to selected cell");
    require(!transactionService.addTransaction(
                owner->getUserId(), cellId, TransactionType::EXPENSE, "Cross-cell", 1,
                "2026-07-03", travelCategory->getCategoryId()),
            "cross-cell category assignment rejected");
    require(!transactionService.deleteTransaction(
                owner->getUserId(), cellId, secondCellTransaction->getTransactionId()),
            "transaction delete is scoped to selected cell");
    require(!transactionService.deleteTransaction(
                member->getUserId(), cellId, income->getTransactionId()),
            "member cannot delete another user's transaction");
    require(!transactionService.deleteTransaction(
                guest->getUserId(), cellId, expense->getTransactionId()),
            "guest cannot delete transactions");
    require(transactionService.deleteTransaction(
                member->getUserId(), cellId, expense->getTransactionId()),
            "member deletes own transaction");
    require(!transactions.findTransactionById(expense->getTransactionId()),
            "deleted transaction removed");

    require(cellService.updateCell(
                member->getUserId(), cellId, "Denied", "Denied") == CellOperationResult::NOT_AUTHORIZED,
            "member cannot edit cell");
    require(cellService.updateCell(
                owner->getUserId(), cellId, "Updated Budget", "Updated description") == CellOperationResult::SUCCESS,
            "owner edits cell");
    require(cellService.removeMemberFromCell(
                owner->getUserId(), cellId, owner->getUserId()) == CellOperationResult::CANNOT_MODIFY_OWNER,
            "owner cannot be removed");
    require(cellService.removeMemberFromCell(
                owner->getUserId(), cellId, guest->getUserId()) == CellOperationResult::SUCCESS,
            "owner removes guest");

    require(cellService.deleteCell(owner->getUserId(), cellId) == CellOperationResult::SUCCESS, "owner deletes cell");
    require(!cells.findCellById(cellId), "cell deleted");
    require(!transactions.findTransactionById(income->getTransactionId()),
            "cell deletion cascades transactions");
}
