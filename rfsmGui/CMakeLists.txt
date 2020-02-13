#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

if(ENABLE_RFSMGUI)
    include(GNUInstallDirs)

    find_package(Graphviz REQUIRED)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

    if(USE_YARP)
        # Find YARP (optional)
        find_package(YARP 3.1.101 REQUIRED)
    endif()

    #########################################################################
    # Control where libraries and executables are placed during the build
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

    add_subdirectory(QGVCore)
    add_subdirectory(src)

endif(ENABLE_RFSMGUI)

