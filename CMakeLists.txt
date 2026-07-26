cmake_minimum_required(VERSION 3.16)

project(FinanCell 
    VERSION 0.1.0 
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

add_executable(FinanCell 
    src/main.cpp
    src/ui/console/ConsoleUI.cpp
    src/domain/User.cpp
    src/domain/FinancialCell.cpp
    src/application/UserService.cpp
    src/storage/sqlite/SQLiteDatabase.cpp
    src/storage/sqlite/SQLiteMigrations.cpp
    src/storage/sqlite/SQLiteStatement.cpp
    src/storage/sqlite/SQLiteUserRepository.cpp
    src/storage/sqlite/SQLiteCellRepository.cpp
    src/application/CellService.cpp
    src/application/TransactionService.cpp
    src/utils/StringUtils.cpp
)

target_include_directories(FinanCell
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(FinanCell
    PRIVATE
    SQLite::SQLite3
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(FinanCell
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        )
endif()
