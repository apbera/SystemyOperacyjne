cmake_minimum_required(VERSION 2.8)
project(zad2 C)

set(CMAKE_C_FLAGS "-std=gnu11")

add_compile_options("-Wall")

add_executable(zad2 main.c)

add_executable(tester tester.c)