#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ../librFSM/include)

if(NOT TARGET rFSMTools::rFSM)
  find_package(rFSMTools REQUIRED)
endif()

add_executable(rfsmTest main.cpp )

target_link_libraries(rfsmTest rFSMTools::rFSM)

install(TARGETS rfsmTest
        EXPORT rfsmTest
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
