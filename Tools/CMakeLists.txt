cmake_minimum_required(VERSION 3.19.5)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

project(Tools CXX)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fdiagnostics-color=always)
include_directories(.)

file(GLOB_RECURSE LIBELF_SOURCES LibElf/*.cpp)

add_library(LibElf ${LIBELF_SOURCES})
target_link_libraries(LibElf fmt)

add_executable(EmbedFile EmbedFile.cpp FileSystem.cpp)
target_link_libraries(EmbedFile bsd LibElf)
