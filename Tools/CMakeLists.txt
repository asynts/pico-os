cmake_minimum_required(VERSION 3.19.5)
project(Tools CXX)

find_package(fmt)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
target_compile_options(project_options INTERFACE -fdiagnostics-color=always -g)
target_include_directories(project_options INTERFACE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/..)
target_compile_definitions(project_options INTERFACE HOST)

file(GLOB LibElf_SOURCES LibElf/*.cpp)
add_library(LibElf ${LibElf_SOURCES})
target_link_libraries(LibElf project_options fmt::fmt)

file(GLOB ElfEmbed_SOURCES *.cpp)
add_executable(ElfEmbed ${ElfEmbed_SOURCES})
target_link_libraries(ElfEmbed project_options LibElf bsd)
