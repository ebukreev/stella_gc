cmake_minimum_required(VERSION 3.29)
project(stella_gc C)

set(CMAKE_C_STANDARD 11)

add_executable(stella_gc main.c
        gc.c
        gc.h
        runtime.c
        runtime.h)
