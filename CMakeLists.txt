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

option(FINANCELL_BUILD_GUI "Build the Qt Quick graphical application" ON)

if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation"
        VERBATIM
    )
endif()

set(FINANCELL_CORE_SOURCES
    src/domain/User.cpp
    src/domain/FinancialCell.cpp
    src/application/UserService.cpp
    src/application/CategoryService.cpp
    src/application/MonthlyReportService.cpp
    src/storage/sqlite/SQLiteDatabase.cpp
    src/storage/sqlite/SQLiteMigrations.cpp
    src/storage/sqlite/SQLiteStatement.cpp
    src/storage/sqlite/SQLiteUserRepository.cpp
    src/storage/sqlite/SQLiteCellRepository.cpp
    src/storage/sqlite/SQLiteCategoryRepository.cpp
    src/storage/sqlite/SQLiteTransactionRepository.cpp
    src/application/CellService.cpp
    src/application/TransactionService.cpp
    src/utils/StringUtils.cpp
    src/utils/DateUtils.cpp
    src/security/PasswordHasher.cpp
)

add_library(FinanCellCore STATIC ${FINANCELL_CORE_SOURCES})
target_include_directories(FinanCellCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(FinanCellCore PUBLIC SQLite::SQLite3 PRIVATE ${SODIUM_LIBRARY})

add_library(FinanCellConsoleUI STATIC src/ui/console/ConsoleUI.cpp)
target_include_directories(FinanCellConsoleUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(FinanCellConsoleUI PUBLIC FinanCellCore)

add_executable(FinanCell src/main.cpp)
target_link_libraries(FinanCell PRIVATE FinanCellConsoleUI)

function(financell_enable_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    endif()
endfunction()

financell_enable_warnings(FinanCellCore)
financell_enable_warnings(FinanCellConsoleUI)
financell_enable_warnings(FinanCell)

if(FINANCELL_BUILD_GUI)
    find_package(Qt6 6.2 QUIET COMPONENTS Quick Qml QuickControls2)
    if(Qt6_FOUND)
        set(FINANCELL_QML_FILES
            src/ui/qt/qml/Main.qml
            src/ui/qt/qml/LoginPage.qml
            src/ui/qt/qml/RegisterPage.qml
            src/ui/qt/qml/HomePage.qml
        )
        set_source_files_properties(src/ui/qt/qml/Main.qml
            PROPERTIES QT_RESOURCE_ALIAS Main.qml)
        set_source_files_properties(src/ui/qt/qml/LoginPage.qml
            PROPERTIES QT_RESOURCE_ALIAS LoginPage.qml)
        set_source_files_properties(src/ui/qt/qml/RegisterPage.qml
            PROPERTIES QT_RESOURCE_ALIAS RegisterPage.qml)
        set_source_files_properties(src/ui/qt/qml/HomePage.qml
            PROPERTIES QT_RESOURCE_ALIAS HomePage.qml)

        qt_add_executable(FinanCellGui
            src/gui_main.cpp
            src/ui/qt/controllers/UserController.cpp
            src/ui/qt/controllers/UserController.h
        )
        qt_add_qml_module(FinanCellGui
            URI FinanCell
            VERSION 1.0
            RESOURCE_PREFIX /qt/qml
            QML_FILES ${FINANCELL_QML_FILES}
        )
        target_link_libraries(FinanCellGui PRIVATE
            FinanCellCore
            Qt6::Quick
            Qt6::Qml
            Qt6::QuickControls2
        )
        set_target_properties(FinanCellGui PROPERTIES AUTOMOC ON AUTORCC ON)
        financell_enable_warnings(FinanCellGui)
    else()
        message(WARNING
            "Qt 6.2 or newer with Quick, Qml, and QuickControls2 was not found. "
            "FinanCellGui will not be built; the console application and tests remain available."
        )
    endif()
endif()

if(BUILD_TESTING)
    add_executable(FinanCellTests
        tests/CellOperationsTests.cpp
    )
    target_link_libraries(FinanCellTests PRIVATE FinanCellCore)
    financell_enable_warnings(FinanCellTests)

    add_test(NAME CellOperations COMMAND FinanCellTests)

    add_executable(FinanCellPersistenceTests tests/PersistenceAndMigrationTests.cpp)
    target_link_libraries(FinanCellPersistenceTests PRIVATE FinanCellCore)
    financell_enable_warnings(FinanCellPersistenceTests)
    add_test(NAME PersistenceAndMigration COMMAND FinanCellPersistenceTests)

    add_executable(FinanCellConsoleTests tests/ConsoleFlowTests.cpp)
    target_link_libraries(FinanCellConsoleTests PRIVATE FinanCellConsoleUI)
    financell_enable_warnings(FinanCellConsoleTests)
    add_test(NAME ConsoleFlow COMMAND FinanCellConsoleTests)
endif()
