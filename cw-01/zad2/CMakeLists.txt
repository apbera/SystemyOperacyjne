cmake_minimum_required(VERSION 2.8)
project(zad2 C)

set(CMAKE_C_STANDARD 11)
add_compile_options("-Wall")

add_library(lib_zad2 library.c)

add_executable(zad2 main.c)

target_link_libraries(zad2 lib_zad2)