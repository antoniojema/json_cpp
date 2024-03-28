# CMake minimum version check
cmake_minimum_required(VERSION 3.18)

#C++ standard required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ExeName "json_from_file")

add_executable(${ExeName} main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)

target_link_libraries(${ExeName} ${LibName})
