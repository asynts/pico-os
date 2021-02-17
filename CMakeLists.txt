cmake_minimum_required(VERSION 3.19.5)

include(CMake/pico_sdk_import.cmake)

project(Pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_executable(Kernel Kernel/main.cpp)
target_link_libraries(Kernel pico_stdlib)

pico_add_extra_outputs(Kernel)
