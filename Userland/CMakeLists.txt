file(GLOB_RECURSE LIBC_SOURCES CONFIGURE_DEPENDS LibC/*.c)

add_custom_target(Shell.elf ALL
    COMMAND arm-none-eabi-gcc -std=c11 -fpic -o ${CMAKE_CURRENT_BINARY_DIR}/Shell.elf
        -T ${CMAKE_CURRENT_SOURCE_DIR}/Userland.x -Xlinker --orphan-handling=warn
        -I${CMAKE_CURRENT_SOURCE_DIR}/LibC
        ${LIBC_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/Shell.c)

add_custom_target(ShellEmbedded.1.o ALL
    COMMAND arm-none-eabi-ld -r -b binary -L ${CMAKE_CURRENT_BINARY_DIR} Shell.elf -o ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.1.o
    DEPENDS Shell.elf)

add_custom_target(ShellEmbedded.2.o ALL
    COMMAND arm-none-eabi-objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.1.o ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.2.o
    DEPENDS ShellEmbedded.1.o)

add_library(ShellEmbedded OBJECT IMPORTED GLOBAL)
set_property(TARGET ShellEmbedded PROPERTY IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.2.o)
