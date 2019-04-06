cmake_minimum_required(VERSION 2.8)
project(zad1 C)

set(CMAKE_C_STANDARD 11)
add_compile_options("-Wall")

add_library(lib_zad1_static STATIC library.c)
add_library(lib_zad1_shared SHARED library.c)