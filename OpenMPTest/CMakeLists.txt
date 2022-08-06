cmake_minimum_required(VERSION 3.22)
project(openmptest C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fopenmp")

add_executable(openmptest main.c)
