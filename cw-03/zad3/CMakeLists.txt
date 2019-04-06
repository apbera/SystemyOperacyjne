cmake_minimum_required(VERSION 2.8)
project(zad3 C)

set(CMAKE_C_FLAGS "-std=gnu11")

add_compile_options("-Wall")

add_executable(zad3 main.c)

add_executable(tester tester.c)