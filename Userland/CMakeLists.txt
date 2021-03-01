file(GLOB_RECURSE LIBC_SOURCES CONFIGURE_DEPENDS LibC/*.c)

add_library(LibC_ ${LIBC_SOURCES})
target_include_directories(LibC_ BEFORE PUBLIC LibC)

add_library(LibC INTERFACE)
target_link_libraries(LibC INTERFACE c LibC_)

add_executable(Shell Shell.c)
target_link_libraries(Shell LibC)
