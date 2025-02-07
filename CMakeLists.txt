cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# project settings
project(wordle
    VERSION     0.1.0
    DESCRIPTION "Wordle App in C++"
)

# external libraries
include(cmake/CPM.cmake)
CPMAddPackage("gh:boostorg/preprocessor#boost-1.86.0")
CPMAddPackage("gh:aufam/delameta#main")
CPMAddPackage("gh:ArthurSonzogni/ftxui@5.0.0")
CPMAddPackage("gh:catchorg/Catch2@3.7.0")

# lib
file(GLOB_RECURSE SOURCES src/*)
add_library(wordle ${SOURCES})
target_include_directories(wordle PUBLIC include)

target_link_libraries(wordle PUBLIC
    Boost::preprocessor
    fmt-header-only
    delameta
)

target_compile_options(wordle PRIVATE
    -Wall
    -Wextra
    -pedantic
)

# main
file(GLOB_RECURSE SOURCES app/*)
add_executable(main ${SOURCES})
set_target_properties(main PROPERTIES OUTPUT_NAME wordle)

target_link_libraries(main PRIVATE
    wordle
    ftxui::dom
    ftxui::component
    ftxui::screen
)

target_compile_options(main PRIVATE
    -Wall
    -Wextra
    -pedantic
)

# test
file(GLOB_RECURSE SOURCES test/*)
add_executable(test_all ${SOURCES})

target_link_libraries(test_all PRIVATE
    wordle
    Catch2::Catch2WithMain
)

target_compile_options(test_all PRIVATE
    -Wall
    -Wextra
    -pedantic
)
