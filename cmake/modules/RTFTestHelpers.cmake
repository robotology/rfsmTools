# RTFTestHelpers
# -------------------------
#
# Helpers functions for creating tests

#=============================================================================
# Copyright 2017 Ali Paikan <ali.paikan@gmail.com>
#

include(CTest)
include(CMakeParseArguments)

macro(ENABLE_RTF_TESTS)
    
    enable_testing()

    # add the required cmake packages
    find_package(RTF REQUIRED)
    find_package(RTF 1.1 COMPONENTS DLL)

    include_directories(${CMAKE_SOURCE_DIR}
                        ${RTF_INCLUDE_DIRS}
                        ${CMAKE_SOURCE_DIR}/librFSM/include)

    set(TESTRUNNER_PATH testrunner)
    # set the output plugin directory to collect all tests
    set(TEST_TARGET_PATH ${CMAKE_BINARY_DIR}/bin/tests)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TEST_TARGET_PATH})
    set(CMAKE_SHARED_MODULE_PREFIX "")
endmacro()

function(ADD_RTF_CPPTEST)
    # set(options )
    set(oneValueArgs NAME PARAM ENV)
    set(multiValueArgs SRCS LIBS)
    cmake_parse_arguments(ADD_RTF_CPPTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(TEST_LIBS rFSM ${RTF_LIBRARIES} ${ADD_RTF_CPPTEST_LIBS})

    add_library(${ADD_RTF_CPPTEST_NAME} MODULE ${ADD_RTF_CPPTEST_SRCS})
    target_link_libraries(${ADD_RTF_CPPTEST_NAME} ${TEST_LIBS})

    # adding test unit
     add_test(NAME ${ADD_RTF_CPPTEST_NAME}
         COMMAND ${TESTRUNNER_PATH} -v --no-output --no-summary -p ${ADD_RTF_CPPTEST_PARAM} -e "${ADD_RTF_CPPTEST_ENV}" --test $<TARGET_FILE:${ADD_RTF_CPPTEST_NAME}>
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

macro(ADD_RTF_TEST_SCRIPT SOURCE)
    configure_file(${SOURCE} ${TEST_TARGET_PATH}/${SOURCE} COPYONLY)
    # adding test unit
    add_test(NAME ${SOURCE}
         COMMAND ${TESTRUNNER_PATH} -v --no-output --no-summary --test ${TEST_TARGET_PATH}/${SOURCE}
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endmacro()

macro(ADD_RTF_LUATEST SOURCE)
    ADD_RTF_TEST_SCRIPT(${SOURCE})
endmacro()

macro(ADD_RTF_PYTHONTEST SOURCE)
    ADD_RTF_TEST_SCRIPT(${SOURCE})
endmacro()

macro(ADD_RTF_RUBYTEST SOURCE)
    ADD_RTF_TEST_SCRIPT(${SOURCE})
endmacro()
