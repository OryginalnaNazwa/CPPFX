cmake_minimum_required(VERSION 3.23)
project(CPPFX VERSION 0.12.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_IS_TOP_LEVEL)
    set(CPPFX_EXAMPLES_DEFAULT ON)
else()
    set(CPPFX_EXAMPLES_DEFAULT OFF)
endif()
option(CPPFX_BUILD_EXAMPLES "Build the bundled examples" ${CPPFX_EXAMPLES_DEFAULT})

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
file(GLOB CPPFX_SOURCES CONFIGURE_DEPENDS "cppfx/source/*.cpp")
file(GLOB CPPFX_HEADERS CONFIGURE_DEPENDS "cppfx/include/CPPFX/*.hpp")

add_library(CPPFX STATIC ${CPPFX_SOURCES})
add_library(CPPFX::CPPFX ALIAS CPPFX)

target_sources(CPPFX PUBLIC
    FILE_SET HEADERS
    BASE_DIRS cppfx/include
    FILES ${CPPFX_HEADERS})

target_link_libraries(CPPFX PUBLIC raylib)

target_compile_options(CPPFX PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>)

# --- examples -----------------------------------------------------------
if(CPPFX_BUILD_EXAMPLES)
    file(GLOB example_dirs CONFIGURE_DEPENDS "examples/*")
    foreach(dir ${example_dirs})
        if(IS_DIRECTORY "${dir}")
            file(GLOB sources CONFIGURE_DEPENDS "${dir}/*.cpp")
            if(sources)
                get_filename_component(name "${dir}" NAME)
                string(MAKE_C_IDENTIFIER "${name}" target_name)
                add_executable(example_${target_name} ${sources})
                target_link_libraries(example_${target_name} PRIVATE CPPFX::CPPFX)
            endif()
        endif()
    endforeach()
endif()

# --- install ------------------------------------------------------------
install(TARGETS CPPFX
        ARCHIVE DESTINATION lib COMPONENT cppfx
        FILE_SET HEADERS DESTINATION include COMPONENT cppfx)
install(FILES README.md LICENSE DESTINATION . COMPONENT cppfx)
