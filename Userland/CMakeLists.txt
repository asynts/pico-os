find_package(Tools MODULE REQUIRED)

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

add_custom_target(Shell.2.elf ALL
    COMMAND arm-none-eabi-objcopy
        --strip-unneeded
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.1.elf
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.2.elf
    DEPENDS Shell.1.elf)

add_custom_target(EmbeddedFiles.elf ALL
    COMMAND ${CMAKE_BINARY_DIR}/Tools/ElfEmbed
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.2.elf /bin/Shell.elf
        -o ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedFiles.elf
    DEPENDS ElfEmbed Shell.2.elf)

add_library(EmbeddedFiles OBJECT IMPORTED GLOBAL)
set_property(TARGET EmbeddedFiles PROPERTY IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedFiles.elf)
