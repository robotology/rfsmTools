#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECTNAME rfsmTest)
PROJECT(${PROJECTNAME})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ../librFSM/include)

find_package(rFSM REQUIRED)

add_executable(${PROJECTNAME} main.cpp )

target_link_libraries(${PROJECTNAME} rFSMTools::rFSM)

#install(TARGETS ${PROJECTNAME}
#        EXPORT ${PROJECTNAME}
#        COMPONENT runtime
#        RUNTIME DESTINATION bin)
