#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

cmake_minimum_required(VERSION 3.0.0)

project(rFSMTools
        LANGUAGES C CXX
        VERSION 1.0.0)


# enable ctest
enable_testing()

#add some options
option (EMBED_RFSM  "Embed rfsm lua file into the librFSM (optional)" TRUE)
option (ENABLE_RFSMGUI  "build rfsm simulator" TRUE)
option (BUILD_TESTING   "build tests" FALSE)
option (USE_YARP "Use YARP (optional)" FALSE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# setting default compilation to release/optmized
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, recommanded options are: Debug or Release" FORCE)
endif()

# enable RPATH
option(ENABLE_RPATH "Enable RPATH for this library" ON)
mark_as_advanced(ENABLE_RPATH)
include(AddInstallRPATHSupport)
include(RFSMTOOLSDoc)

add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
                          LIB_DIRS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
                          DEPENDS ENABLE_RPATH
                          USE_LINK_PATH)


add_subdirectory(librFSM)
add_subdirectory(examples)
add_subdirectory(rfsmGui)
add_subdirectory(tests)

include(AddUninstallTarget)
