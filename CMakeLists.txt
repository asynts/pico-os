# ==============================================================================
# PicoOS - CMake Build Configuration
# ==============================================================================
#
# Build targets:
#   - Kernel.1.elf  : Main kernel binary with debug symbols
#   - Kernel.1.uf2  : UF2 firmware for direct flashing
#   - Kernel.elf    : Stripped kernel for production
#
# Usage:
#   mkdir build && cd build
#   cmake .. -G Ninja
#   ninja
#
# ==============================================================================

cmake_minimum_required(VERSION 3.19.5)

# ------------------------------------------------------------------------------
# Build Configuration
# ------------------------------------------------------------------------------

# Export compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ------------------------------------------------------------------------------
# Pico SDK Setup
# ------------------------------------------------------------------------------

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
find_package(Tools MODULE)

# Import Pico SDK (will download if PICO_SDK_PATH not set)
include(CMake/pico_sdk_import.cmake)

# ------------------------------------------------------------------------------
# Project Definition
# ------------------------------------------------------------------------------

project(PicoOS
    VERSION 0.4.0
    DESCRIPTION "A microkernel-inspired OS for the Raspberry Pi Pico"
    LANGUAGES C CXX ASM
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Initialize Pico SDK
pico_sdk_init()

# ------------------------------------------------------------------------------
# Compiler Options
# ------------------------------------------------------------------------------

# Warnings to suppress from Pico SDK headers (not our code)
set(SDK_WARNINGS_OFF
    -Wno-unused-parameter
    -Wno-type-limits
)

set(SDK_CXX_WARNINGS_OFF
    -Wno-reorder
    -Wno-ignored-qualifiers
)

# Common compile options interface
add_library(project_options INTERFACE)

target_compile_options(project_options INTERFACE
    -fdiagnostics-color=always
    -Wall
    -Wextra
    -Werror
    -O0
    -fstack-protector-strong
    ${SDK_WARNINGS_OFF}
)

target_compile_options(project_options INTERFACE
    $<$<COMPILE_LANGUAGE:CXX>:
        -frtti
        ${SDK_CXX_WARNINGS_OFF}
    >
)

target_include_directories(project_options INTERFACE
    ${CMAKE_SOURCE_DIR}
)

# ------------------------------------------------------------------------------
# Userland Applications
# ------------------------------------------------------------------------------

add_subdirectory(Userland)

# ------------------------------------------------------------------------------
# Kernel Build
# ------------------------------------------------------------------------------

file(GLOB_RECURSE KERNEL_SOURCES CONFIGURE_DEPENDS
    Kernel/*.cpp
    Kernel/*.S
    Std/*.cpp
)

add_executable(Kernel.1 ${KERNEL_SOURCES})

target_link_libraries(Kernel.1
    pico_stdlib
    pico_bootrom
    hardware_dma
    project_options
    LibEmbeddedFiles
)

target_compile_definitions(Kernel.1 PRIVATE
    KERNEL
)

# Use custom linker script with alignment fixes
pico_set_linker_script(Kernel.1 ${CMAKE_CURRENT_SOURCE_DIR}/Kernel/linker.ld)

# Generate .uf2, .bin, .hex outputs
pico_add_extra_outputs(Kernel.1)

# ------------------------------------------------------------------------------
# Stripped Kernel for Production
# ------------------------------------------------------------------------------

add_custom_target(Kernel.elf ALL
    COMMAND arm-none-eabi-objcopy
        --strip-symbol=__flash_data_2
        --strip-symbol=__flash_data_3
        --strip-symbol=__flash_data_4
        --strip-symbol=__flash_data_5
        --strip-symbol=__flash_base
        Kernel.1.elf Kernel.elf
    DEPENDS Kernel.1
    COMMENT "Creating stripped Kernel.elf"
)

# ------------------------------------------------------------------------------
# Summary
# ------------------------------------------------------------------------------

message(STATUS "--------------------------------------------------------------")
message(STATUS "PicoOS v${PROJECT_VERSION}")
message(STATUS "  Platform:    RP2040 (Raspberry Pi Pico)")
message(STATUS "  Build type:  ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Standard:  ${CMAKE_C_STANDARD}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "--------------------------------------------------------------")
