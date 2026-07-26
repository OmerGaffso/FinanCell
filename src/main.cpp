#include <iostream>
#include <exception>
#include <filesystem>

#include "ui/console/ConsoleUI.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteCategoryRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteTransactionRepository.h"
#include "storage/sqlite/SQLiteUserRepository.h"
#include "security/PasswordHasher.h"

int main()
{
    try
    {
        std::filesystem::create_directories("data");

        SQLiteDatabase database("data/financell.db");
        SQLiteMigrations::apply(database);

        SQLiteUserRepository userRepository(database);
        SodiumPasswordHasher passwordHasher;
        UserService userService{userRepository, passwordHasher};
        SQLiteCellRepository cellRepository(database);
        CellService cellService{cellRepository, userRepository};
        SQLiteCategoryRepository categoryRepository(database);
        CategoryService categoryService{categoryRepository, cellRepository};
        SQLiteTransactionRepository transactionRepository(database);
        TransactionService transactionService{
            transactionRepository, cellRepository, categoryRepository};
        MonthlyReportService monthlyReportService{transactionRepository, cellRepository};

        ConsoleUI ui{
            userService,
            cellService,
            categoryService,
            transactionService,
            monthlyReportService};
        ui.runApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Application Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
