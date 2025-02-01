# Project name
project(simple_snake_game)

# Set cmake minimum version
cmake_minimum_required(VERSION 3.15)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/sdl2")

# Add source files from ./source
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/source/*.cpp")

# Create the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Add SDL2 library
find_package(SDL2 REQUIRED)

# Platform-specific configurations
if (WIN32)
    # Additional configuration for Windows
    ### NOT YET IMPLEMENTED ###
elseif (UNIX)
    # Link SDL2 libraries and include directories
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} pthread)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
endif()
