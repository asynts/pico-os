file(GLOB_RECURSE LIBC_SOURCES CONFIGURE_DEPENDS LibC/*.c)

add_library(LibC_ ${LIBC_SOURCES})
target_include_directories(LibC_ BEFORE PUBLIC LibC)
target_compile_options(LibC_ PRIVATE -fpic)

add_library(LibC INTERFACE)
target_link_libraries(LibC INTERFACE c LibC_)

add_executable(Shell Shell.c)
target_link_libraries(Shell LibC)
target_compile_options(Shell PRIVATE -fpie)

add_custom_target(ShellEmbedded.1.o ALL
    COMMAND arm-none-eabi-ld -r -b binary -L ${CMAKE_CURRENT_BINARY_DIR} Shell.elf -o ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.1.o
    DEPENDS Shell)

add_custom_target(ShellEmbedded.2.o ALL
    COMMAND arm-none-eabi-objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.1.o ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.2.o
    DEPENDS ShellEmbedded.1.o)

add_library(ShellEmbedded OBJECT IMPORTED GLOBAL)
set_property(TARGET ShellEmbedded PROPERTY IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.2.o)
