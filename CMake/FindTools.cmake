if(NOT DEFINED Tools_FOUND)
    set(Tools_FOUND TRUE)

    include(ExternalProject)
    ExternalProject_Add(Tools
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/Tools
        BINARY_DIR ${CMAKE_BINARY_DIR}/Tools
        INSTALL_COMMAND ""
        BUILD_ALWAYS TRUE)

    set(ELF_EMBED_EXECUTABLE ${CMAKE_BINARY_DIR}/Tools/ElfEmbed)

    add_custom_target(ElfEmbed)
    add_dependencies(ElfEmbed Tools)
endif()
