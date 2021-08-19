
cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default to Debug
if(NOT CMAKE_BUILD_TYPE)
    message("Build type not specified, using Debug")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type not specified, using Debug" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

project(pybind11_example LANGUAGES CXX)

include_directories(.)

add_subdirectory(third_party/gtest)
add_subdirectory(third_party/pybind11)
add_subdirectory(bindings)
add_subdirectory(cpp)
add_subdirectory(tests)
