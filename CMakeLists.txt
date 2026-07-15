cmake_minimum_required(version 3.16)

project(FinanCell version 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(FinanCell 
    main.cpp
    ui/console/consoleUI.cpp
)

target_include_directories(FinanCell
    PRIVATE
    ${CMAKE_CURRENT_SOURC_DIR}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(FinanCell
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()