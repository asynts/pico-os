# FIXME: This means that we don't have dependency checking for ElfEmbed.
add_executable(ElfEmbed IMPORTED)
set_target_properties(ElfEmbed PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Tools/Build/ElfEmbed)

file(GLOB_RECURSE SHELL_SOURCES CONFIGURE_DEPENDS LibC/*.c LibC/*.S Shell.c)

add_custom_target(Shell.1.elf ALL
    COMMAND clang --target=arm-none-eabi -mcpu=cortex-m0plus
        -std=gnu11 -Og -g -static -nostdlib -fcolor-diagnostics
        -fropi -frwpi
        -I ${CMAKE_CURRENT_SOURCE_DIR}/LibC
        -I ${CMAKE_SOURCE_DIR}/Kernel/Interface
        -T ${CMAKE_CURRENT_SOURCE_DIR}/Userland.x
        ${SHELL_SOURCES}
        -o ${CMAKE_CURRENT_BINARY_DIR}/Shell.1.elf)

add_custom_target(Shell.elf ALL
    COMMAND arm-none-eabi-objcopy
        --strip-unneeded
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.1.elf
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.elf
    DEPENDS Shell.1.elf)

# FIXME: It seems CMake doesn't like it when you create object files it doesn't know about.

add_custom_target(EmbeddedFiles.1.o ALL
    COMMAND ${CMAKE_SOURCE_DIR}/Tools/Build/ElfEmbed
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.elf /bin/Shell.elf
        -o ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedFiles.1.o
    DEPENDS ElfEmbed Shell.elf)

add_custom_target(EmbeddedFiles.o ALL
    COMMAND arm-none-eabi-ld -r
        -T ${CMAKE_CURRENT_SOURCE_DIR}/EmbedHack.x
        ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedFiles.1.o
        -o ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedFiles.o
    DEPENDS EmbeddedFiles.1.o)

add_library(LibEmbeddedFiles.1 OBJECT IMPORTED GLOBAL)
set_target_properties(LibEmbeddedFiles.1
    PROPERTIES
        IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedFiles.o)
add_dependencies(LibEmbeddedFiles.1 EmbeddedFiles.o)

add_library(LibEmbeddedFiles $<TARGET_OBJECTS:LibEmbeddedFiles.1>)
set_target_properties(LibEmbeddedFiles
    PROPERTIES
        LINKER_LANGUAGE CXX)
