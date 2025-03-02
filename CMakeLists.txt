cmake_minimum_required(VERSION 3.28)
project(random_generator C)

set(CMAKE_C_STANDARD 11)

add_executable(random_generator main.c
        tsp_utilities.c
        tsp_utilities.h
        parsing/parsing_util.c
        parsing/parsing_util.h)
