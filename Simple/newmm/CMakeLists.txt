cmake_minimum_required(VERSION 3.22)
project(newmm C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

add_executable(newmm main.c)
