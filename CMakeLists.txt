cmake_minimum_required(VERSION 3.15.0)
# /usr/bin/clang-12
# set(CMAKE_C_COMPILER "/usr/bin/clang-12")
# set(CMAKE_CXX_COMPILER "/usr/bin/clang++-12")
# /usr/bin/gcc-11
set(CMAKE_C_COMPILER "/usr/bin/gcc-11")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-11")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(playground VERSION 0.1.0)

add_compile_options(-std=c++20 -g -ggdb)

include_directories(.)

add_executable(playground main.cpp)

target_link_libraries(playground pthread)