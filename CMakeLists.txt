cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -fsanitize=address,undefined" CACHE STRING "")

project(mycomp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-Wall \
-Wextra \
-Wpedantic \
-Wshadow \
-Wconversion \
-Woverloaded-virtual \
-Wmisleading-indentation \
-Wduplicated-cond \
-Wduplicated-branches \
-Wnull-dereference \
-Wmismatched-tags \
-Warith-conversion \
-Wconversion-null \
-Wcast-qual \
-Wdelete-incomplete \
-Wdelete-non-virtual-dtor \
-Wdisabled-optimization \
-Wempty-body \
-Wlogical-op \
-Wredundant-decls \
-Wsign-promo \
-Wsuggest-override \
-Wswitch-enum \
-Wundef \
-Wunreachable-code \
-Wvexing-parse \
-Wvirtual-move-assign \
-Wzero-as-null-pointer-constant \
-Wnon-virtual-dtor")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-Weverything \
-Wno-disabled-macro-expansion \
-Wno-sign-conversion \
-Wno-old-style-cast \
-Wno-c++98-compat \
-Wno-c++98-compat-pedantic \
-Wno-c++20-compat \
-Wno-weak-vtables \
-Wno-padded \
-Wno-float-equal \
-Wno-ctad-maybe-unsupported \
-Wno-double-promotion \
-Wno-exit-time-destructors \
-Wno-global-constructors \
-Wno-missing-noreturn")

endif()

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

find_package(fmt REQUIRED)
target_compile_definitions(fmt::fmt INTERFACE FMT_ENFORCE_COMPILE_STRING)
add_library(fmt ALIAS fmt::fmt)

find_package(magic_enum REQUIRED)
add_library(magic_enum ALIAS magic_enum::magic_enum)

find_package(Boost REQUIRED)
add_library(boost ALIAS Boost::boost)

find_package(Catch2 REQUIRED)

add_subdirectory(mycomp)
add_subdirectory(tests)
