cmake_minimum_required(VERSION 3.16)

project(FinanCell 
    VERSION 0.1.0 
    LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD_17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)
find_library(SODIUM_LIBRARY NAMES sodium libsodium.so.23 REQUIRED)
find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation"
        VERBATIM
    )
endif()

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
    src/storage/sqlite/SQLiteTransactionRepository.cpp
    src/application/CellService.cpp
    src/application/TransactionService.cpp
    src/utils/StringUtils.cpp
    src/security/PasswordHasher.cpp
)

target_include_directories(FinanCell
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(FinanCell
    PRIVATE
    SQLite::SQLite3
    ${SODIUM_LIBRARY}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(FinanCell
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        )
endif()

if(BUILD_TESTING)
    add_executable(FinanCellTests
        tests/CellOperationsTests.cpp
        src/domain/User.cpp
        src/domain/FinancialCell.cpp
        src/application/UserService.cpp
        src/application/CellService.cpp
        src/application/TransactionService.cpp
        src/storage/sqlite/SQLiteDatabase.cpp
        src/storage/sqlite/SQLiteMigrations.cpp
        src/storage/sqlite/SQLiteStatement.cpp
        src/storage/sqlite/SQLiteUserRepository.cpp
        src/storage/sqlite/SQLiteCellRepository.cpp
        src/storage/sqlite/SQLiteTransactionRepository.cpp
        src/utils/StringUtils.cpp
        src/security/PasswordHasher.cpp
    )

    target_include_directories(FinanCellTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(FinanCellTests PRIVATE SQLite::SQLite3 ${SODIUM_LIBRARY})

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(FinanCellTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME CellOperations COMMAND FinanCellTests)
endif()
