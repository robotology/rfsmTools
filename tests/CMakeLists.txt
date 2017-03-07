#  rFSM Tools
#  Copyright: (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
#  Authors: Ali Paikan <ali.paikan@gmail.com>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#


if (BUILD_TESTING)

    include(RTFTestHelpers)

    # enable RTF testing
    ENABLE_RTF_TESTS()

    # adding librFSM tests
    add_subdirectory(librFSM)

endif()

