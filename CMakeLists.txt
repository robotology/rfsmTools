# Copyright: 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECTNAME rFSM)
PROJECT(${PROJECTNAME})

#add some options
option (EMBED_RFSM  "Embed rfsm lua file into the librFSM" TRUE)
option (ENABLE_SIMRFSM  "build rfsm simulator" TRUE)

add_subdirectory(librFSM)
add_subdirectory(examples)
add_subdirectory(rfsmGui)

