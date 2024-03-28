# CMake minimum version check
cmake_minimum_required(VERSION 3.18)

#C++ standard required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Global variables
set(ProjectName "json_cpp")
set(LibName     "json_cpp")

# Project name
project(${ProjectName})

# Include source code
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

option(JSON_BUILD_EXAMPLES "JSON_BUILD_EXAMPLES" OFF)
if(JSON_BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()
