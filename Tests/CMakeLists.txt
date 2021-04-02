cmake_minimum_required(VERSION 3.19.5)
project(Tests CXX)

enable_testing()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
target_compile_options(project_options INTERFACE -fdiagnostics-color=always -g)
target_compile_definitions(project_options INTERFACE TEST)
target_include_directories(project_options INTERFACE ${CMAKE_SOURCE_DIR}/..)

file(GLOB_RECURSE Std_SOURCES CONFIGURE_DEPENDS ../Std/*.cpp)
add_library(LibStd ${Std_SOURCES})
target_link_libraries(LibStd project_options)

file(GLOB Tests_SOURCES CONFIGURE_DEPENDS *.cpp)

foreach(source ${Tests_SOURCES})
    get_filename_component(name ${source} NAME_WE)

    add_executable(${name} ${source})
    target_link_libraries(${name} LibStd project_options)
endforeach()
