#include <iostream>
#include <exception>
#include <filesystem>

#include "ui/console/ConsoleUI.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteUserRepository.h"

int main()
{
    try
    {
        std::filesystem::create_directories("data");

        SQLiteDatabase database("data/financell.db");
        SQLiteMigrations::apply(database);

        SQLiteUserRepository userRepository(database);
        UserService userService{userRepository};
        SQLiteCellRepository cellRepository(database);
        CellService cellService{cellRepository};

        ConsoleUI ui{userService, cellService};
        ui.runApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Application Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
