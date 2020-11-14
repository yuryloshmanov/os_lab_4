cmake_minimum_required(VERSION 3.17)
project(lab4 C)

set(CMAKE_C_STANDARD 11)

add_executable(main main.c shared.h)
add_executable(child child.c shared.h)
