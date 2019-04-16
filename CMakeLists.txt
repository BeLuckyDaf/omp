cmake_minimum_required(VERSION 3.13)
project(Code C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(Main
        src/netpbm.c
        src/netpbm.h)

add_executable(Test
        src/netpbm.c
        src/netpbm.h tests/netpbm_test.c)