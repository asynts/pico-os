cmake_minimum_required(VERSION 3.19.5)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-fdiagnostics-color=always)

add_subdirectory(Tools)

include(CMake/pico_sdk_import.cmake)

project(Pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

include_directories(.)

add_subdirectory(Userland)

file(GLOB_RECURSE KERNEL_SOURCES CONFIGURE_DEPENDS Kernel/*.cpp Std/*.cpp)

add_executable(Kernel ${KERNEL_SOURCES})
target_link_libraries(Kernel pico_stdlib)
pico_enable_stdio_uart(Kernel 1)

pico_add_extra_outputs(Kernel)

target_link_libraries(Kernel ShellEmbedded)
