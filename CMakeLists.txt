cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -fsanitize=address,undefined" CACHE STRING "")

project(mycomp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

find_package(fmt REQUIRED)
target_compile_definitions(fmt::fmt INTERFACE FMT_ENFORCE_COMPILE_STRING)
add_library(fmt ALIAS fmt::fmt)

find_package(Catch2 REQUIRED)

add_subdirectory(mycomp)
add_subdirectory(tests)
