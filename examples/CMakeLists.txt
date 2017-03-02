# Copyright: 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECTNAME rfsmTest)
PROJECT(${PROJECTNAME})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ../librFSM/include)

add_executable(${PROJECTNAME} main.cpp )

target_link_libraries(${PROJECTNAME} rFSM)

install(TARGETS ${PROJECTNAME} RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../bin)


