# We can not use add_executable here because CMake wasn't build with multiple toolchains in mind.
add_custom_target(sdebug ALL
    COMMAND cc -Wall -Wextra -std=c11 ${CMAKE_CURRENT_SOURCE_DIR}/sdebug.c -o ${CMAKE_CURRENT_BINARY_DIR}/sdebug
    SOURCES sdebug.c)
