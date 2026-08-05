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
            src/ui/qt/qml/CellListPage.qml
            src/ui/qt/qml/CreateCellPage.qml
            src/ui/qt/qml/CellDashboardPage.qml
            src/ui/qt/qml/MemberPage.qml
            src/ui/qt/qml/CategoryPage.qml
            src/ui/qt/qml/TransactionPage.qml
            src/ui/qt/qml/TransactionFormPage.qml
            src/ui/qt/qml/MonthlyReportPage.qml
            src/ui/qt/qml/CellSettingsPage.qml
            src/ui/qt/qml/UserLookupPage.qml
            src/ui/qt/qml/BrandPalette.qml
        )
        set_source_files_properties(src/ui/qt/qml/Main.qml
            PROPERTIES QT_RESOURCE_ALIAS Main.qml)
        set_source_files_properties(src/ui/qt/qml/LoginPage.qml
            PROPERTIES QT_RESOURCE_ALIAS LoginPage.qml)
        set_source_files_properties(src/ui/qt/qml/RegisterPage.qml
            PROPERTIES QT_RESOURCE_ALIAS RegisterPage.qml)
        set_source_files_properties(src/ui/qt/qml/HomePage.qml
            PROPERTIES QT_RESOURCE_ALIAS HomePage.qml)
        set_source_files_properties(src/ui/qt/qml/CellListPage.qml
            PROPERTIES QT_RESOURCE_ALIAS CellListPage.qml)
        set_source_files_properties(src/ui/qt/qml/CreateCellPage.qml
            PROPERTIES QT_RESOURCE_ALIAS CreateCellPage.qml)
        set_source_files_properties(src/ui/qt/qml/CellDashboardPage.qml
            PROPERTIES QT_RESOURCE_ALIAS CellDashboardPage.qml)
        set_source_files_properties(src/ui/qt/qml/MemberPage.qml
            PROPERTIES QT_RESOURCE_ALIAS MemberPage.qml)
        set_source_files_properties(src/ui/qt/qml/CategoryPage.qml
            PROPERTIES QT_RESOURCE_ALIAS CategoryPage.qml)
        set_source_files_properties(src/ui/qt/qml/TransactionPage.qml
            PROPERTIES QT_RESOURCE_ALIAS TransactionPage.qml)
        set_source_files_properties(src/ui/qt/qml/TransactionFormPage.qml
            PROPERTIES QT_RESOURCE_ALIAS TransactionFormPage.qml)
        set_source_files_properties(src/ui/qt/qml/MonthlyReportPage.qml
            PROPERTIES QT_RESOURCE_ALIAS MonthlyReportPage.qml)
        set_source_files_properties(src/ui/qt/qml/CellSettingsPage.qml
            PROPERTIES QT_RESOURCE_ALIAS CellSettingsPage.qml)
        set_source_files_properties(src/ui/qt/qml/UserLookupPage.qml
            PROPERTIES QT_RESOURCE_ALIAS UserLookupPage.qml)
        set_source_files_properties(src/ui/qt/qml/BrandPalette.qml
            PROPERTIES QT_RESOURCE_ALIAS BrandPalette.qml)
        set_source_files_properties("data/assets/FinanCell Icon.png"
            PROPERTIES QT_RESOURCE_ALIAS FinanCellIcon.png)
        set_source_files_properties("data/assets/FinanCell Banner Light.png"
            PROPERTIES QT_RESOURCE_ALIAS FinanCellBannerLight.png)

        qt_add_executable(FinanCellGui
            src/gui_main.cpp
            src/ui/qt/controllers/CellController.cpp
            src/ui/qt/controllers/CellController.h
            src/ui/qt/controllers/CategoryController.cpp
            src/ui/qt/controllers/CategoryController.h
            src/ui/qt/controllers/MemberController.cpp
            src/ui/qt/controllers/MemberController.h
            src/ui/qt/controllers/TransactionController.cpp
            src/ui/qt/controllers/TransactionController.h
            src/ui/qt/controllers/ReportController.cpp
            src/ui/qt/controllers/ReportController.h
            src/ui/qt/controllers/UserController.cpp
            src/ui/qt/controllers/UserController.h
            src/ui/qt/session/SessionState.cpp
            src/ui/qt/session/SessionState.h
        )
        set_target_properties(FinanCellGui PROPERTIES AUTOMOC ON AUTORCC ON)
        qt_add_qml_module(FinanCellGui
            URI FinanCell
            VERSION 1.0
            RESOURCE_PREFIX /qt/qml
            OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/qml/FinanCell
            QML_FILES ${FINANCELL_QML_FILES}
        )
        qt_add_resources(FinanCellGui financell_brand_assets
            PREFIX /assets
            FILES
                "data/assets/FinanCell Icon.png"
                "data/assets/FinanCell Banner Light.png"
        )
        target_link_libraries(FinanCellGui PRIVATE
            FinanCellCore
            Qt6::Quick
            Qt6::Qml
            Qt6::QuickControls2
        )
        financell_enable_warnings(FinanCellGui)

        if(BUILD_TESTING)
            add_executable(FinanCellQtCellControllerTests
                tests/QtCellControllerTests.cpp
                src/ui/qt/controllers/CellController.cpp
                src/ui/qt/controllers/CellController.h
                src/ui/qt/controllers/CategoryController.cpp
                src/ui/qt/controllers/CategoryController.h
                src/ui/qt/controllers/MemberController.cpp
                src/ui/qt/controllers/MemberController.h
                src/ui/qt/controllers/TransactionController.cpp
                src/ui/qt/controllers/TransactionController.h
                src/ui/qt/controllers/ReportController.cpp
                src/ui/qt/controllers/ReportController.h
                src/ui/qt/session/SessionState.cpp
                src/ui/qt/session/SessionState.h
            )
            set_target_properties(FinanCellQtCellControllerTests PROPERTIES AUTOMOC ON)
            target_include_directories(FinanCellQtCellControllerTests
                PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
            target_link_libraries(FinanCellQtCellControllerTests PRIVATE
                FinanCellCore
                Qt6::Core
            )
            financell_enable_warnings(FinanCellQtCellControllerTests)
            add_test(NAME QtCellController COMMAND FinanCellQtCellControllerTests)
        endif()
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
