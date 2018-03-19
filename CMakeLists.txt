cmake_minimum_required(VERSION 3.8)
project(CHIP-8-cpp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp chip8.cpp chip8.h)

add_executable(CHIP-8-cpp ${SOURCE_FILES})