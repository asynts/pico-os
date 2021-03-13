cmake_minimum_required(VERSION 3.19.5)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

project(Tools CXX)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fdiagnostics-color=always)

add_executable(EmbedFile EmbedFile.cpp)
