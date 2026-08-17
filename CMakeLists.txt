# CPPFX - build definition.
#
#   cmake -B build -DCMAKE_BUILD_TYPE=Release   # configure, once
#   cmake --build build                         # compile
#   ctest --test-dir build --output-on-failure  # run the tests
#   cmake --install build --prefix release/CPPFX --component cppfx
#
# Only re-run the configure step when this file changes.

cmake_minimum_required(VERSION 3.23)
project(CPPFX VERSION 0.19.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)          # -std=c++23, not -std=gnu++23

# Examples and tests are built when CPPFX is the project being built, and
# skipped when someone pulls it into their own build as a subdirectory -
# nobody wants your examples compiling inside their game.
if(PROJECT_IS_TOP_LEVEL)
    set(CPPFX_EXTRAS_DEFAULT ON)
else()
    set(CPPFX_EXTRAS_DEFAULT OFF)
endif()
option(CPPFX_BUILD_EXAMPLES "Build the bundled examples" ${CPPFX_EXTRAS_DEFAULT})
option(CPPFX_BUILD_TESTS    "Build the test suite"       ${CPPFX_EXTRAS_DEFAULT})

# --- raylib ---------------------------------------------------------------
# Use an installed raylib if there is one, otherwise download and build it.
# That is what lets CI work on a clean machine with nothing preinstalled.

find_package(raylib 6.0 QUIET)
if(NOT raylib_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)   # don't build raylib's examples
    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        6.0
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(raylib)
endif()

# --- the library ----------------------------------------------------------

# CONFIGURE_DEPENDS makes CMake re-glob when files appear, so adding a
# widget does not mean editing this file.
file(GLOB CPPFX_SOURCES CONFIGURE_DEPENDS "cppfx/source/*.cpp")
file(GLOB CPPFX_HEADERS CONFIGURE_DEPENDS "cppfx/include/CPPFX/*.hpp")

add_library(CPPFX STATIC ${CPPFX_SOURCES})

# The namespaced alias is what consumers link against. Using it internally
# too means a typo is an error at configure time rather than a confusing
# link failure later.
add_library(CPPFX::CPPFX ALIAS CPPFX)

# FILE_SET HEADERS with BASE_DIRS cppfx/include is what makes
# #include <CPPFX/CPPFX.hpp> resolve for anything linking this target - and
# it keeps working after install, with no hand-written include paths.
target_sources(CPPFX PUBLIC
    FILE_SET HEADERS
    BASE_DIRS cppfx/include
    FILES ${CPPFX_HEADERS})

target_link_libraries(CPPFX PUBLIC raylib)

# PRIVATE: your warnings, not your users'.
target_compile_options(CPPFX PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>)

# --- tests ----------------------------------------------------------------
# Every .cpp in tests/ is compiled into one binary. Tests register
# themselves, so a new file needs no changes here or in main.cpp.

if(CPPFX_BUILD_TESTS)
    enable_testing()
    file(GLOB CPPFX_TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
    if(CPPFX_TEST_SOURCES)
        add_executable(cppfx_tests ${CPPFX_TEST_SOURCES})
        target_link_libraries(cppfx_tests PRIVATE CPPFX::CPPFX)
        add_test(NAME cppfx_tests COMMAND cppfx_tests)
    endif()
endif()

# --- examples -------------------------------------------------------------
# One executable per folder under examples/, built from every .cpp in it.

if(CPPFX_BUILD_EXAMPLES)
    file(GLOB example_dirs CONFIGURE_DEPENDS "examples/*")
    foreach(dir ${example_dirs})
        if(IS_DIRECTORY "${dir}")
            file(GLOB sources CONFIGURE_DEPENDS "${dir}/*.cpp")
            if(sources)
                get_filename_component(name "${dir}" NAME)
                # Folder names can contain spaces; target names cannot.
                string(MAKE_C_IDENTIFIER "${name}" target_name)
                add_executable(example_${target_name} ${sources})
                target_link_libraries(example_${target_name} PRIVATE CPPFX::CPPFX)
            endif()
        endif()
    endforeach()
endif()

# --- install --------------------------------------------------------------
# COMPONENT cppfx keeps raylib's own install rules out of the release tree.
# Install with:  cmake --install build --prefix <dir> --component cppfx

install(TARGETS CPPFX
        ARCHIVE DESTINATION lib COMPONENT cppfx
        FILE_SET HEADERS DESTINATION include COMPONENT cppfx)
install(FILES README.md LICENSE DESTINATION . COMPONENT cppfx)
