cmake_minimum_required(VERSION 4.3)
project(Know_Your_Computer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Know_Your_Computer
        src/main.cpp
        include/system/component-reader.h
        include/system/proc-reader.h
        src/system/proc-reader.cpp
        src/system/gpu-reader.cpp
        include/system/gpu-reader.h)

target_include_directories(Know_Your_Computer PRIVATE
        ${CMAKE_SOURCE_DIR}/include/system
)