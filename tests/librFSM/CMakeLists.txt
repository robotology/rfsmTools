#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#


# LoadrFSM
ADD_RTF_CPPTEST(NAME LoadrFSM
                SRCS loadrFSM.cpp
                PARAM "${CMAKE_SOURCE_DIR}/tests/fsm/simple_fsm.lua")

