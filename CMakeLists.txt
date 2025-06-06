cmake_minimum_required(VERSION 3.31)
project(cycle_detection_experiments)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(cycle_detection_experiments
        src/main.cpp
        src/message.h
        src/node.cpp
        src/node.h
        src/util.cpp
        src/util.h)
