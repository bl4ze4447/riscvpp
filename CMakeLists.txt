cmake_minimum_required(VERSION 4.1)
project(riscvpp)

set(CMAKE_CXX_STANDARD 23)

add_executable(riscvpp main.cpp
        src/core/cpu.cpp
        include/riscvpp/core/cpu.h
        include/riscvpp/core/register.h
)

target_include_directories(riscvpp PRIVATE include)

enable_testing()

add_executable(test_register tests/core/register_tests.cpp)
add_test(NAME RegisterTest COMMAND test_register)

target_include_directories(test_register PRIVATE include)
