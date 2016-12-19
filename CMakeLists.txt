# Copyright: 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECTNAME rFSM)
PROJECT(${PROJECTNAME})

#middleware specific options
find_package(YARP)
# automatically build YARP integration if yarp is found
option (ENABLE_YARP "Enable yarp integration" ${YARP_FOUND})
option (EMBED_RFSM  "Embed rfsm lua file into the librFSM" FALSE)

# build yarp specific extensions
if (ENABLE_YARP)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
    set_property(GLOBAL PROPERTY LIBRFSM_HAS_YARP TRUE)
else()
    set_property(GLOBAL PROPERTY LIBRFSM_HAS_YARP FALSE)
endif()

add_subdirectory(librFSM)
add_subdirectory(examples)

