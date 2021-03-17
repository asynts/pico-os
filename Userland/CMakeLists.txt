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

add_custom_target(EmbeddedFiles.o ALL
    COMMAND ${CMAKE_SOURCE_DIR}/Tools/Build/ElfEmbed
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.elf /bin/Shell.elf
        -o ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedFiles.o
    DEPENDS ElfEmbed Shell.elf)

add_library(LibEmbeddedFiles OBJECT IMPORTED GLOBAL)
set_target_properties(LibEmbeddedFiles
    PROPERTIES
        IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedFiles.o)
add_dependencies(LibEmbeddedFiles EmbeddedFiles.o)
