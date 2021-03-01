file(GLOB_RECURSE LIBC_SOURCES CONFIGURE_DEPENDS LibC/*.c)

add_library(LibC_ ${LIBC_SOURCES})
target_include_directories(LibC_ BEFORE PUBLIC LibC)

add_library(LibC INTERFACE)
target_link_libraries(LibC INTERFACE c LibC_)

add_executable(Shell Shell.c)
target_link_libraries(Shell LibC)

add_custom_target(ShellEmbedded.o ALL
    COMMAND arm-none-eabi-ld -r -b binary -L ${CMAKE_CURRENT_BINARY_DIR} Shell.elf -o ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.o
    DEPENDS Shell)

add_library(ShellEmbedded OBJECT IMPORTED GLOBAL)
set_property(TARGET ShellEmbedded PROPERTY IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/ShellEmbedded.o)
