#include <iostream>
#include <exception>
#include <filesystem>

#include "ui/console/ConsoleUI.h"
#include "storage/sqlite/SQLiteUserRepository.h"

int main()
{
    try
    {
        std::filesystem::create_directories("data");

        SQLiteUserRepository userRepository("data/financell.db");
        userRepository.initializeDatabase();

        UserService userService{userRepository};

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
