cmake_minimum_required(VERSION 3.19.5)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)

include(CMake/pico_sdk_import.cmake)

project(Pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_library(project_options INTERFACE)
target_compile_options(project_options INTERFACE -fdiagnostics-color=always)
target_include_directories(project_options INTERFACE ${CMAKE_SOURCE_DIR})

add_subdirectory(Userland)

file(GLOB_RECURSE Kernel_SOURCES CONFIGURE_DEPENDS Kernel/*.cpp Std/*.cpp)

add_executable(Kernel ${Kernel_SOURCES})
target_link_libraries(Kernel pico_stdlib project_options LibEmbeddedFiles)
pico_enable_stdio_uart(Kernel 1)
pico_add_extra_outputs(Kernel)
