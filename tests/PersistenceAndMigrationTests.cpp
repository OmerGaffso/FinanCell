#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "application/CategoryService.h"
#include "application/CellService.h"
#include "application/MonthlyReportService.h"
#include "application/PersistenceError.h"
#include "application/TransactionService.h"
#include "application/UserService.h"
#include "security/PasswordHasher.h"
#include "storage/sqlite/Schema.h"
#include "storage/sqlite/SQLiteCategoryRepository.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteStatement.h"
#include "storage/sqlite/SQLiteTransactionRepository.h"
#include "storage/sqlite/SQLiteUserRepository.h"

namespace
{
void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}

class TemporaryDirectory
{
public:
    TemporaryDirectory()
    {
        const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path = std::filesystem::temp_directory_path() /
                 ("financell-persistence-test-" + std::to_string(suffix));
        std::filesystem::create_directories(m_path);
    }

    ~TemporaryDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    const std::filesystem::path& path() const { return m_path; }

private:
    std::filesystem::path m_path;
};

void createVersionFourFixture(SQLiteDatabase& database)
{
    database.execute(Schema::CREATE_USERS_TABLE);
    database.execute(Schema::CREATE_CELLS_TABLE);
    database.execute(Schema::CREATE_CELL_MEMBERS_TABLE);
    database.execute(Schema::CREATE_CELL_MEMBERS_USER_INDEX);
    database.execute(Schema::CREATE_CELL_OWNER_MEMBERSHIP_TRIGGER);
    database.execute(Schema::CREATE_TRANSACTIONS_TABLE);
    database.execute(Schema::CREATE_TRANSACTIONS_CELL_INDEX);
    database.execute(Schema::ADD_TRANSACTION_CATEGORY);
    database.execute("PRAGMA user_version = 4;");

    database.execute(
        "INSERT INTO users (id, username, display_name, password_hash) "
        "VALUES (1, 'owner', 'Owner User', 'not-used'), "
        "(2, 'outsider', 'Outside User', 'not-used');");
    database.execute(
        "INSERT INTO cells (id, name, description, currency, owner_user_id) "
        "VALUES (7, 'Legacy Cell', '', 'ILS', 1), "
        "(8, 'Second Cell', '', 'ILS', 1);");
    database.execute(
        "INSERT INTO transactions "
        "(id, cell_id, created_by_user_id, type, description, amount_minor, "
        " occurred_at, created_at, updated_at, category) VALUES "
        "(10, 7, 1, 'INCOME', 'Legacy income', 10000, "
        " '2025-01-02', '2025-01-02 10:00:00', '2025-01-02 10:00:00', 'Food'), "
        "(11, 7, 1, 'EXPENSE', 'Legacy expense', 2500, "
        " '2025-01-03', '2025-01-03 10:00:00', '2025-01-03 10:00:00', 'food');");
}

void testVersionFourMigration()
{
    SQLiteDatabase database(":memory:");
    createVersionFourFixture(database);
    SQLiteMigrations::apply(database);
    SQLiteMigrations::apply(database);

    SQLiteStatement version(database, "PRAGMA user_version;");
    require(version.next() && version.columnUInt64(0) == 5, "schema upgraded to v5");

    SQLiteCategoryRepository categories(database);
    SQLiteTransactionRepository transactions(database);
    const auto cellSevenCategories = categories.findCategoriesByCellId(7);
    require(cellSevenCategories.size() == 2, "General and canonical legacy category exist");
    const auto food = categories.findCategoryByName(7, "FOOD");
    require(food.has_value(), "legacy category lookup is case insensitive");
    bool rejectedDuplicateCategory = false;
    try
    {
        categories.insertCategory(Category(0, 7, "food"));
    }
    catch (const PersistenceError&)
    {
        rejectedDuplicateCategory = true;
    }
    require(rejectedDuplicateCategory, "database rejects case-insensitive category duplicate");

    const auto migratedIncome = transactions.findTransactionById(10);
    const auto migratedExpense = transactions.findTransactionById(11);
    require(migratedIncome && migratedExpense, "legacy transaction IDs preserved");
    require(migratedIncome->getCategoryId() == food->getCategoryId() &&
                migratedExpense->getCategoryId() == food->getCategoryId(),
            "legacy category case variants share one category");
    require(migratedIncome->getOccurredAt() == "2025-01-02",
            "legacy transaction date preserved");
    SQLiteStatement timestamps(
        database,
        "SELECT created_at, updated_at FROM transactions WHERE id = 10;");
    require(timestamps.next() && timestamps.columnText(0) == "2025-01-02 10:00:00" &&
                timestamps.columnText(1) == "2025-01-02 10:00:00",
            "legacy transaction timestamps preserved");

    const auto otherGeneral = categories.findCategoryByName(8, "General");
    require(otherGeneral.has_value(), "existing empty cell receives General category");
    bool rejectedCrossCellCategory = false;
    try
    {
        transactions.insertTransaction(Transaction(
            0, 8, 1, TransactionType::EXPENSE, "Wrong category", 100,
            "2025-01-04", food->getCategoryId()));
    }
    catch (const PersistenceError&)
    {
        rejectedCrossCellCategory = true;
    }
    require(rejectedCrossCellCategory, "database rejects cross-cell category assignment");

    bool rejectedNonMemberCreator = false;
    try
    {
        transactions.insertTransaction(Transaction(
            0, 7, 2, TransactionType::EXPENSE, "Outside creator", 100,
            "2025-01-04", food->getCategoryId()));
    }
    catch (const PersistenceError&)
    {
        rejectedNonMemberCreator = true;
    }
    require(rejectedNonMemberCreator, "database rejects nonmember transaction creator");

    SQLiteStatement foreignKeyCheck(database, "PRAGMA foreign_key_check;");
    require(!foreignKeyCheck.next(), "migrated schema has no foreign-key violations");

    SQLiteCellRepository cells(database);
    require(cells.deleteCell(7), "delete migrated cell");
    require(categories.findCategoriesByCellId(7).empty(), "category cascade works");
    require(!transactions.findTransactionById(10), "transaction cascade works");
}

void testFileBackedReopen()
{
    TemporaryDirectory directory;
    const auto databasePath = directory.path() / "financell.db";
    std::uint64_t userId = 0;
    std::uint64_t cellId = 0;

    {
        SQLiteDatabase database(databasePath.string());
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

        std::string username = "persistent";
        std::string displayName = "Persistent User";
        std::string password = "secret55";
        require(userService.createUser(username, displayName, password), "persist user");
        userId = users.findUserByUsername(username)->getUserId();
        require(cellService.createCell("Persistent Cell", userId, "Reopen test"), "persist cell");
        cellId = cellService.getCellsForUser(userId).front().getCellId();
        require(categoryService.createCategory(userId, cellId, "Salary") ==
                    CategoryOperationResult::SUCCESS,
                "persist category");
        const auto category = categories.findCategoryByName(cellId, "Salary");
        require(transactionService.addTransaction(
                    userId, cellId, TransactionType::INCOME, "Pay", 12345,
                    "2025-06-15", category->getCategoryId()).has_value(),
                "persist transaction");
    }

    {
        SQLiteDatabase database(databasePath.string());
        SQLiteMigrations::apply(database);
        SQLiteUserRepository users(database);
        SQLiteCellRepository cells(database);
        SQLiteCategoryRepository categories(database);
        SQLiteTransactionRepository transactions(database);
        SodiumPasswordHasher hasher;
        UserService userService(users, hasher);
        MonthlyReportService reportService(transactions, cells);

        require(userService.authenticateUser("PERSISTENT", "secret55").has_value(),
                "reopened user authenticates");
        require(cells.findCellById(cellId).has_value(), "reopened cell exists");
        require(categories.findCategoryByName(cellId, "salary").has_value(),
                "reopened category exists");
        const auto report = reportService.generate(userId, cellId, "2025-06");
        require(report && report->totalIncomeInMinorUnits == 12345 &&
                    report->totalExpensesInMinorUnits == 0 &&
                    report->balanceInMinorUnits == 12345 &&
                    report->categories.size() == 1,
                "reopened report contains persisted data");
    }
}
}

int main()
{
    testVersionFourMigration();
    testFileBackedReopen();
}
