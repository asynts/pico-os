file(GLOB_RECURSE LIBC_SOURCES CONFIGURE_DEPENDS LibC/*.c LibC/*.S)
file(GLOB_RECURSE Example_SOURCES CONFIGURE_depends LibC/*.c LibC/*.S)

function(userland_executable name_)
    add_custom_target(${name_}.1.elf ALL
        COMMAND clang --target=arm-none-eabi -mcpu=cortex-m0plus
            -std=gnu11 -Og -g -static -nostdlib -fcolor-diagnostics
            -fropi -frwpi
            -DUSERLAND
            -I ${CMAKE_CURRENT_SOURCE_DIR}/LibC
            -I ${CMAKE_SOURCE_DIR}
            -T ${CMAKE_CURRENT_SOURCE_DIR}/Userland.x
            -Xlinker --nmagic
            --sysroot=/usr/local/arm-none-eabi
            ${LIBC_SOURCES}
            ${CMAKE_CURRENT_SOURCE_DIR}/${name_}.c
            -o ${CMAKE_CURRENT_BINARY_DIR}/${name_}.1.elf)

    add_custom_target(${name_}.elf ALL
        COMMAND arm-none-eabi-objcopy
            --strip-unneeded
            ${CMAKE_CURRENT_BINARY_DIR}/${name_}.1.elf
            ${CMAKE_CURRENT_BINARY_DIR}/${name_}.elf
        DEPENDS ${name_}.1.elf)

    list(APPEND USERLAND_EXECUTABLES ${name_}.elf)
endfunction()

userland_executable(Shell)
userland_executable(Example)
userland_executable(Editor)

add_custom_target(FileSystem.elf ALL
    COMMAND ${ELF_EMBED_EXECUTABLE}
    DEPENDS ${USERLAND_EXECUTABLES} ElfEmbed)

add_library(LibEmbeddedFiles OBJECT IMPORTED GLOBAL)
set_target_properties(LibEmbeddedFiles
    PROPERTIES
        IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/FileSystem.elf)
add_dependencies(LibEmbeddedFiles FileSystem.elf)
