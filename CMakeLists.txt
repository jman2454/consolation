cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(game VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add the executable
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(game ${SOURCES})

# Optionally set the binary output directory
set_target_properties(game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)