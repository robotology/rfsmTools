#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#


if (BUILD_TESTING)

    find_package(RobotTestingFramework 2.0.0 REQUIRED)
    include(RobotTestingFrameworkTestHelpers)

    # enable RTF testing
    ENABLE_ROBOTTESTINGFRAMEWORK_TESTS()

    # adding librFSM tests
    add_subdirectory(librFSM)

endif()

