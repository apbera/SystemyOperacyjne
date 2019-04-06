cmake_minimum_required(VERSION 2.8)
project(zad3 C)

set(CMAKE_C_STANDARD 11)

option(O0 "O0" OFF)
option(O3 "O3" OFF)
option(Os "Os" OFF)

if(Os)
    add_compile_options("-Os")
endif(Os)

if(O0)
    add_compile_options("-O0")
endif(O0)

if(O3)
    add_compile_options("-O3")
endif(O3)

add_compile_options("-Wall")

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")


add_library(zad3_lib_static STATIC library.c)
add_executable(zad3_static main.c)
target_link_libraries(zad3_static zad3_lib_static)

add_library(zad3_lib_shared SHARED library.c)
add_executable(zad3_shared main.c)
target_link_libraries(zad3_shared zad3_lib_shared)

add_executable(zad3_dynamic main_dynamic.c)
target_link_libraries(zad3_dynamic zad3_lib_shared)
target_link_libraries(zad3_dynamic dl)

set(RESULTS_FILE results3b.txt)

set(SMALL_TEST create_table 10 search_directory . test.txt tmp.txt add_block remove_block 0 add_block remove_block 0)
set(MEDIUM_TEST create_table 10 search_directory ~ test.txt tmp.txt add_block remove_block 0 add_block remove_block 0)
set(BIG_TEST create_table 10 search_directory / test.txt tmp.txt add_block remove_block 0 add_block remove_block 0)

add_custom_command(TARGET zad3_static POST_BUILD COMMAND echo "---------Static Tests---------" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_static POST_BUILD COMMAND echo "Small Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_static POST_BUILD COMMAND ./zad3_static ${SMALL_TEST})
add_custom_command(TARGET zad3_static POST_BUILD COMMAND echo "Medium Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_static POST_BUILD COMMAND ./zad3_static ${MEDIUM_TEST})
add_custom_command(TARGET zad3_static POST_BUILD COMMAND echo "Big Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_static POST_BUILD COMMAND ./zad3_static ${BIG_TEST})
add_custom_command(TARGET zad3_shared POST_BUILD COMMAND echo)
add_custom_command(TARGET zad3_shared POST_BUILD COMMAND echo "---------Shared Tests---------" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_shared POST_BUILD COMMAND echo "Small Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_shared POST_BUILD COMMAND ./zad3_shared ${SMALL_TEST})
add_custom_command(TARGET zad3_shared POST_BUILD COMMAND echo "Medium Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_shared POST_BUILD COMMAND ./zad3_shared ${MEDIUM_TEST})
add_custom_command(TARGET zad3_shared POST_BUILD COMMAND echo "Big Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_shared POST_BUILD COMMAND ./zad3_shared ${BIG_TEST})
add_custom_command(TARGET zad3_dynamic POST_BUILD COMMAND echo)
add_custom_command(TARGET zad3_dynamic POST_BUILD COMMAND echo "---------Dynamic Tests---------" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_dynamic POST_BUILD COMMAND echo "Small Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_dynamic POST_BUILD COMMAND ./zad3_dynamic ${SMALL_TEST})
add_custom_command(TARGET zad3_dynamic POST_BUILD COMMAND echo "Medium Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_dynamic POST_BUILD COMMAND ./zad3_dynamic ${MEDIUM_TEST})
add_custom_command(TARGET zad3_dynamic POST_BUILD COMMAND echo "Big Test" >> ${RESULTS_FILE})
add_custom_command(TARGET zad3_dynamic POST_BUILD COMMAND ./zad3_dynamic ${BIG_TEST})

