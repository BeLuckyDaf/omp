cmake_minimum_required(VERSION 3.13)
project(Code C)

set(CMAKE_C_STANDARD 11)

find_package(Threads)

include_directories(src)

add_executable(Main
        src/netpbm.c
        src/netpbm.h
        src/sobel.c
        src/sobel.h)

add_executable(Test
        src/netpbm.c
        src/netpbm.h
        src/sobel.c
        src/sobel.h
        tests/netpbm_test.c)

target_link_libraries(Test m ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Main m ${CMAKE_THREAD_LIBS_INIT})