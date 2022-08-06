cmake_minimum_required(VERSION 3.22)
project(matmultc_new C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

add_executable(matmultc_new main.c)
