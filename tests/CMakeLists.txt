#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#


if (BUILD_TESTING)

    include(RTFTestHelpers)

    # enable RTF testing
    ENABLE_RTF_TESTS()

    # adding librFSM tests
    add_subdirectory(librFSM)

endif()

