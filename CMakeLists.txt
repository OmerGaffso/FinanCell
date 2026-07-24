cmake_minimum_required(VERSION 3.16)

project(FinanCell 
    VERSION 0.1.0 
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

add_executable(FinanCell 
main.cpp
ui/console/consoleUI.cpp
domain/User.cpp
services/user/UserService.cpp
storage/sqlite/SQLiteStorage.h
storage/sqlite/SQLiteStorage.cpp
)

target_include_directories(FinanCell
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
