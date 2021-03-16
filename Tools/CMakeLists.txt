cmake_minimum_required(VERSION 3.19.5)
project(Tools CXX)

find_package(fmt)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
target_compile_options(project_options INTERFACE -fdiagnostics-color=always)
target_include_directories(project_options INTERFACE ${CMAKE_SOURCE_DIR})

file(GLOB LibElf_SOURCES LibElf/*.cpp)
add_library(LibElf ${LibElf_SOURCES})
target_link_libraries(LibElf project_options fmt::fmt)

file(GLOB EmbedFile_SOURCES *.cpp)
add_executable(EmbedFile ${EmbedFile_SOURCES})
target_link_libraries(EmbedFile LibElf bsd)

# FIXME: Export EmbedFile
