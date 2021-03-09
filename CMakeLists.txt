cmake_minimum_required(VERSION 3.19.5)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-fdiagnostics-color=always)

include(CMake/pico_sdk_import.cmake)

project(Pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

include_directories(.)

add_subdirectory(Userland)

file(GLOB_RECURSE KERNEL_SOURCES CONFIGURE_DEPENDS Kernel/*.cpp Std/*.cpp)

add_executable(Kernel.1 ${KERNEL_SOURCES})
target_link_libraries(Kernel.1 pico_stdlib ShellEmbedded)
pico_enable_stdio_uart(Kernel.1 1)
pico_add_extra_outputs(Kernel.1)

add_custom_target(Kernel.2.elf ALL
    COMMAND arm-none-eabi-objcopy
        --strip-symbol=embedded_shell_binary_start
        --strip-symbol=embedded_shell_binary_end
        --strip-symbol=embedded_shell_binary_size
        ${CMAKE_CURRENT_BINARY_DIR}/Kernel.1.elf
        ${CMAKE_CURRENT_BINARY_DIR}/Kernel.2.elf
    DEPENDS Kernel.1)
