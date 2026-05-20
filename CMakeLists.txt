cmake_minimum_required(VERSION 4.1)
project(riscvpp)

set(CMAKE_CXX_STANDARD 23)

add_executable(riscvpp main.cpp
        src/core/cpu.cpp
        include/riscvpp/core/cpu.h
        include/riscvpp/core/Register.h)
target_include_directories(riscvpp PRIVATE include)
