cmake_minimum_required(VERSION 3.16)
project(EvoScripts LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release if no build type is explicitly chosen
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect source and header files (CONFIGURE_DEPENDS auto-detects added/removed files)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Target-based include directory
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# High-pedantic warnings to catch bugs early
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall                    # Standard warnings
        -Wextra                  # Extra warning checks
        -Wpedantic               # Strict ISO C++ compliance
        # -Wshadow                 # Warn if local variable shadows an outer variable
        -Wnon-virtual-dtor       # Warn on virtual class with non-virtual destructor
        -Wuninitialized          # Warn on uninitialized variable usage
    )
endif()

# Optional Sanitizers for debugging memory issues and undefined behavior
option(ENABLE_SANITIZERS "Enable Address and UndefinedBehavior sanitizers" OFF)
if(ENABLE_SANITIZERS)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
endif()
