cmake_minimum_required(VERSION 3.16)

project(cmse
    VERSION 0.1
    DESCRIPTION "Cloud Metadata Storage Engine"
    LANGUAGES CXX
)

# ================================
# Compiler settings
# ================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ================================
# Warnings (VERY important)
# ================================
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

# ================================
# Include paths
# ================================
include_directories(include)

# ================================
# Sources
# ================================
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# ================================
# Executable
# ================================
add_executable(cmse ${SOURCES})

