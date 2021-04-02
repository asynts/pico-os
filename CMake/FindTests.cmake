if(NOT DEFINED Tests_FOUND)
    set(Tests_FOUND TRUE)

    include(ExternalProject)
    ExternalProject_Add(Tests
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/Tests
        BINARY_DIR ${CMAKE_BINARY_DIR}/Tests
        INSTALL_COMMAND ""
        BUILD_ALWAYS TRUE)
endif()
