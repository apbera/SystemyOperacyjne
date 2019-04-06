cmake_minimum_required(VERSION 3.13)
project(zad2 C)

set(CMAKE_C_STANDARD 11)

add_compile_options("-Wall")

add_executable(zad2 main.c)

add_executable(zad2_nftw main_nftw.c)
