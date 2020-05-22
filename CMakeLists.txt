cmake_minimum_required(VERSION 3.15)
project(maman22 C)

set(CMAKE_C_STANDARD 90)

add_executable(Assignment-1 mycomp.c complex.c getch.c)
add_executable(Assignment-1_test test.c complex.c)