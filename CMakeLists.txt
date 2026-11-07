# CPPFX - build definition.
#
#   cmake -B build -DCMAKE_BUILD_TYPE=Release   # configure, once
#   cmake --build build                         # compile
#   ctest --test-dir build --output-on-failure  # run the tests
#   cmake --install build --prefix release/CPPFX --component cppfx
#
# Only re-run the configure step when this file changes.

cmake_minimum_required(VERSION 3.23)

file(STRINGS cppfx/include/CPPFX/CPPFX.hpp cppfx_version_lines
     REGEX "^#define CPPFX_VERSION_(MAJOR|MINOR|PATCH) ")
foreach(line ${cppfx_version_lines})
    string(REGEX MATCH "CPPFX_VERSION_([A-Z]+) +([0-9]+)" _ "${line}")
    set(CPPFX_VERSION_${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
endforeach()

project(CPPFX
        VERSION ${CPPFX_VERSION_MAJOR}.${CPPFX_VERSION_MINOR}.${CPPFX_VERSION_PATCH}
        LANGUAGES CXX)

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
option(CPPFX_WARNINGS_AS_ERRORS "Treat warnings in CPPFX targets as errors" OFF)

# --- warnings -------------------------------------------------------------
# Applied to everything CPPFX builds: the library, tests and examples.

function(cppfx_warnings target)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<BOOL:${CPPFX_WARNINGS_AS_ERRORS}>>:-Werror>)
endfunction()

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

file(GLOB CPPFX_SOURCES CONFIGURE_DEPENDS "cppfx/source/*.cpp")
file(GLOB CPPFX_HEADERS CONFIGURE_DEPENDS "cppfx/include/CPPFX/*.hpp")

add_library(CPPFX STATIC ${CPPFX_SOURCES})

add_library(CPPFX::CPPFX ALIAS CPPFX)

target_sources(CPPFX PUBLIC
    FILE_SET HEADERS
    BASE_DIRS cppfx/include
    FILES ${CPPFX_HEADERS})

target_link_libraries(CPPFX PUBLIC raylib)

cppfx_warnings(CPPFX)

# --- tests ----------------------------------------------------------------
# Every .cpp in tests/ is compiled into one binary. Tests register
# themselves, so a new file needs no changes here or in main.cpp.

if(CPPFX_BUILD_TESTS)
    enable_testing()

    set(CPPFX_TEST_RUNNER tests/main.cpp tests/harness.cpp)

    # Headless: no window, runs anywhere.
    file(GLOB CPPFX_TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
    list(REMOVE_ITEM CPPFX_TEST_SOURCES
         "${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp"
         "${CMAKE_CURRENT_SOURCE_DIR}/tests/harness.cpp")
    if(CPPFX_TEST_SOURCES)
        add_executable(cppfx_tests ${CPPFX_TEST_RUNNER} ${CPPFX_TEST_SOURCES})
        cppfx_warnings(cppfx_tests)
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
        cppfx_warnings(cppfx_tests_gl)
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
                cppfx_warnings(example_${target_name})
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
