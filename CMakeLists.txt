# CPPFX - build definition.
#
#   cmake -B build -DCMAKE_BUILD_TYPE=Release   # configure, once
#   cmake --build build                         # compile
#   ctest --test-dir build --output-on-failure  # run the tests
#   cmake --install build --prefix release/CPPFX --component cppfx
#
# Only re-run the configure step when this file changes.

cmake_minimum_required(VERSION 3.23)
project(CPPFX VERSION 0.20.1 LANGUAGES CXX)

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

    # Shared runner. Every test file registers itself, so neither binary
    # needs editing when you add one.
    set(CPPFX_TEST_RUNNER tests/main.cpp tests/harness.cpp)

    # Headless: no window, runs anywhere.
    file(GLOB CPPFX_TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
    list(REMOVE_ITEM CPPFX_TEST_SOURCES
         "${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp"
         "${CMAKE_CURRENT_SOURCE_DIR}/tests/harness.cpp")
    if(CPPFX_TEST_SOURCES)
        add_executable(cppfx_tests ${CPPFX_TEST_RUNNER} ${CPPFX_TEST_SOURCES})
        target_include_directories(cppfx_tests PRIVATE tests)
        target_link_libraries(cppfx_tests PRIVATE CPPFX::CPPFX)
        add_test(NAME cppfx_tests COMMAND cppfx_tests)
    endif()

    # Needs a GL context. Separate process, because raylib leaves a stale
    # texture id on the default font after CloseWindow and that would
    # corrupt the headless assertions above.
    file(GLOB CPPFX_GL_TEST_SOURCES CONFIGURE_DEPENDS "tests/gl/*.cpp")
    if(CPPFX_GL_TEST_SOURCES)
        add_executable(cppfx_tests_gl ${CPPFX_TEST_RUNNER} ${CPPFX_GL_TEST_SOURCES})
        target_include_directories(cppfx_tests_gl PRIVATE tests)
        target_link_libraries(cppfx_tests_gl PRIVATE CPPFX::CPPFX)
        # Absolute, so ctest's working directory does not matter.
        target_compile_definitions(cppfx_tests_gl PRIVATE
            CPPFX_TEST_ASSETS="${CMAKE_CURRENT_SOURCE_DIR}/tests/assets")
        add_test(NAME cppfx_tests_gl COMMAND cppfx_tests_gl)
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
