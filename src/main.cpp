#include <iostream>
#include <exception>
#include <filesystem>

#include "ui/console/ConsoleUI.h"
#include "storage/sqlite/SQLiteStorage.h"

int main()
{
    try
    {
        std::filesystem::create_directories("data");

        SQLiteStorage storage("data/financell.db");
        storage.initializeDatabase();

        UserService userService{storage};

        ConsoleUI ui{userService};
        ui.runApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Application Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
