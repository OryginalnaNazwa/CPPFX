cmake_minimum_required(VERSION 3.23)
project(CPPFX VERSION 0.12.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CPPFX_BUILD_TESTS "Build the main.cpp test harness" ON)

# --- raylib -------------------------------------------------------------
find_package(raylib 6.0 QUIET)
if(NOT raylib_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        6.0
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(raylib)
endif()

# --- the library --------------------------------------------------------
add_library(CPPFX STATIC
    cppfx/source/CPPFX.cpp
    cppfx/source/base_item.cpp
    cppfx/source/bases.cpp
    cppfx/source/items.cpp
    cppfx/source/mixins.cpp
    cppfx/source/properties.cpp)

add_library(CPPFX::CPPFX ALIAS CPPFX)

target_sources(CPPFX PUBLIC
    FILE_SET HEADERS
    BASE_DIRS cppfx/include
    FILES
        cppfx/include/CPPFX/CPPFX.hpp
        cppfx/include/CPPFX/base_item.hpp
        cppfx/include/CPPFX/bases.hpp
        cppfx/include/CPPFX/items.hpp
        cppfx/include/CPPFX/mixins.hpp
        cppfx/include/CPPFX/properties.hpp)

target_link_libraries(CPPFX PUBLIC raylib)

target_compile_options(CPPFX PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>)

# --- test harness -------------------------------------------------------
if(CPPFX_BUILD_TESTS)
    add_executable(cppfx_test main.cpp)
    target_link_libraries(cppfx_test PRIVATE CPPFX::CPPFX)
endif()

# --- install (produces your release tree) -------------------------------
install(TARGETS CPPFX
        ARCHIVE DESTINATION lib
        FILE_SET HEADERS DESTINATION include)
install(FILES README.md LICENSE DESTINATION .)
