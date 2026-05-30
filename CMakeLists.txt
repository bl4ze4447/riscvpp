cmake_minimum_required(VERSION 3.31.6)
project(riscvpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(riscv_core
        src/core/cpu.cpp
        include/riscvpp/core/cpu.h
        include/riscvpp/core/register.h
        include/riscvpp/core/instruction.h
)

target_include_directories(riscv_core PUBLIC include)

add_executable(riscvpp main.cpp)
target_link_libraries(riscvpp PRIVATE riscv_core)

enable_testing()

add_executable(test_register tests/core/register_tests.cpp)
target_link_libraries(test_register PRIVATE riscv_core)

add_executable(test_instruction tests/core/instruction_tests.cpp)
target_link_libraries(test_instruction PRIVATE riscv_core)

add_test(NAME RegisterTest COMMAND test_register)