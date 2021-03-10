file(GLOB_RECURSE SHELL_SOURCES CONFIGURE_DEPENDS LibC/*.c LibC/*.S Shell.c)

add_custom_target(Shell.1.elf ALL
    COMMAND clang --target=arm-none-eabi -mcpu=cortex-m0plus
        -std=gnu11 -Og -g -static -nostdlib
        -fropi -frwpi
        -I ${CMAKE_CURRENT_SOURCE_DIR}/LibC
        -T ${CMAKE_CURRENT_SOURCE_DIR}/Userland.x
        ${SHELL_SOURCES}
        -o ${CMAKE_CURRENT_BINARY_DIR}/Shell.1.elf)

add_custom_target(Shell.2.elf ALL
    COMMAND arm-none-eabi-objcopy
        --strip-unneeded
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.1.elf
        ${CMAKE_CURRENT_BINARY_DIR}/Shell.2.elf
    DEPENDS Shell.1.elf)

add_custom_target(ShellEmbedded.1.o ALL
    COMMAND arm-none-eabi-ld -r -b binary
        -L ${CMAKE_CURRENT_BINARY_DIR}
        Shell.2.elf
        -o ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.1.o
    DEPENDS Shell.2.elf)

add_custom_target(ShellEmbedded.2.o ALL
    COMMAND arm-none-eabi-objcopy
        --rename-section .data=.rodata,alloc,load,readonly,data,contents
        --redefine-sym _binary_Shell_2_elf_start=embedded_shell_binary_start
        --redefine-sym _binary_Shell_2_elf_end=embedded_shell_binary_end
        --redefine-sym _binary_Shell_2_elf_size=embedded_shell_binary_size
        ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.1.o
        ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.2.o
    DEPENDS ShellEmbedded.1.o)

add_library(ShellEmbedded OBJECT IMPORTED GLOBAL)
set_property(TARGET ShellEmbedded PROPERTY IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.2.o)
add_definitions(ShellEmbedded ShellEmbedded.2.o)
