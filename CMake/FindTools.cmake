# FIXME: This is a hack, because it doesn't appear to be possible to cross-compile and create
#        host tools at the same time.

if (NOT DEFINED Tools_FOUND)
    include(ExternalProject)

    ExternalProject_Add(Tools
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/Tools
        BINARY_DIR ${CMAKE_BINARY_DIR}/Tools
        BUILD_ALWAYS TRUE
        INSTALL_COMMAND "")

    add_executable(ElfEmbed IMPORTED)
    set_property(TARGET ElfEmbed PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/Tools/ElfEmbed)

    set(Tools_FOUND TRUE)
endif()
