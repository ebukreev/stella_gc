cmake_minimum_required(VERSION 3.29)
project(stella_gc C)

set(CMAKE_C_STANDARD 11)

add_executable(stella_gc
        stella/gc.c
        stella/gc.h
        stella/runtime.c
        stella/runtime.h
out.c)
