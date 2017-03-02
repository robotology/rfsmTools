cmake_minimum_required(VERSION 2.8.9)
set(PROJECTNAME rfsmGui)
project(${PROJECTNAME})


if(ENABLE_SIMRFSM)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
    include(GNUInstallDirs)

    find_package(Graphviz)
    find_package(Qt5Widgets)

    #########################################################################
    # Control where libraries and executables are placed during the build
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

    add_subdirectory(QGVCore)
    add_subdirectory(src)

endif(ENABLE_SIMRFSM)

