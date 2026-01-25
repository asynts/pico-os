cmake_minimum_required(VERSION 3.19.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
find_package(Tools MODULE)

include(/usr/share/pico-sdk/pico_sdk_init.cmake)

project(Pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

pico_sdk_init()

# FIXME: This is mostly necessary for the SDK
set(DISABLE_WARNINGS -Wno-unused-parameter -Wno-type-limits)
set(DISABLE_CXX_WARNINGS -Wno-reorder -Wno-ignored-qualifiers)

add_library(project_options INTERFACE)
target_compile_options(project_options INTERFACE -fdiagnostics-color=always -Wall ${DISABLE_WARNINGS} -Wextra -Werror -O0)
target_compile_definitions(project_options INTERFACE PICO_CXX_DISABLE_ALLOCATION_OVERRIDES=1 LIB_PICO_PRINTF_NONE=1)
target_compile_options(project_options INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-frtti ${DISABLE_CXX_WARNINGS}>)
target_include_directories(project_options INTERFACE ${CMAKE_SOURCE_DIR})

add_subdirectory(Userland)

file(GLOB_RECURSE Kernel_SOURCES CONFIGURE_DEPENDS Kernel/*.cpp Kernel/*.S Std/*.cpp)

add_executable(Kernel ${Kernel_SOURCES})
target_link_libraries(Kernel pico_stdlib pico_bootrom hardware_dma project_options LibEmbeddedFiles)
target_compile_definitions(Kernel PRIVATE KERNEL)
pico_add_extra_outputs(Kernel)
