#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#


# LoadrFSM
include_directories(../../librFSM/include)
ADD_RobotTestingFramework_CPPTEST(NAME LoadrFSMTest
                                  SRCS loadrFSM.cpp
                                  PARAM "${CMAKE_SOURCE_DIR}/tests/fsm/simple_fsm.lua")
