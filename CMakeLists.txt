cmake_minimum_required(VERSION 4.3)
project(Know_Your_Computer)

set(CMAKE_CXX_STANDARD 14)

add_executable(Know_Your_Computer
        src/main.cpp
        include/system/component-reader.h
        include/system/proc-reader.h)
