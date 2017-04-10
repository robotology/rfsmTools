# Copyright: (C) 2017 iCub Facility
# Authors: Ali Paikan
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOX_GENERATE_HTML YES)
    set(DOX_GENERATE_XML NO)
    set(DOX_GENERATE_TAGFILE "")
    option(RFSMTOOLS_DOXYGEN_XML "Generate doxygen XML output, for use by SWIG" NO)
    option(RFSMTOOLS_DOXYGEN_TAGFILE "Generate doxygen tag file, see http://www.doxygen.nl/manual/external.html" NO)
    mark_as_advanced(RFSMTOOLS_DOXYGEN_XML RFSMTOOLS_DOXYGEN_TAGFILE)
    if(RFSMTOOLS_DOXYGEN_XML)
        set(DOX_GENERATE_XML YES)
    endif()
    if(RFSMTOOLS_DOXYGEN_TAGFILE)
        set(DOX_GENERATE_TAGFILE "${CMAKE_BINARY_DIR}/dox/RFSMTOOLS.tag")
    endif()

    set(DOX_FILE Doxyfile.part)
    # RFSMTOOLS documentation is always verbose now.  It used to be
    # partially stripped of stuff less interesting to end-users.
    set(DOX_PATTERNS "*.h *.dox *.cpp")
    set(DOX_GENERATE_MAN NO)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/doxygen/${DOX_FILE}.template
                   ${CMAKE_BINARY_DIR}/dox/${DOX_FILE})
    add_custom_target(dox COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/dox/${DOX_FILE})

    set(DOX_GENERATE_HTML NO)
    set(DOX_GENERATE_MAN YES)
    set(DOX_GENERATE_XML NO)
    set(DOX_GENERATE_TAGFILE "")
    set(DOX_PATTERNS "cmd_*.dox")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/doxygen/${DOX_FILE}.template
                   ${CMAKE_BINARY_DIR}/dox/${DOX_FILE}.man)
    add_custom_target(man COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/dox/${DOX_FILE}.man)

endif(DOXYGEN_FOUND)
