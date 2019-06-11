# Robot Testing Framework
#
# Copyright (C) 2015-2019 Istituto Italiano di Tecnologia (IIT)
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA


# RobotTestingFrameworkTestHelpers
# -------------------------
#
# Helpers functions for creating tests

include(CTest)
include(CMakeParseArguments)

macro(ENABLE_RobotTestingFramework_TESTS)
  set(TESTRUNNER_PATH ${CMAKE_BINARY_DIR}/bin/robottestingframework-testrunner)
  # set the output plugin directory to collect all tests
  set(TEST_TARGET_PATH ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TEST_TARGET_PATH})
  set(CMAKE_SHARED_MODULE_PREFIX "")
  # running test units
  #add_test(NAME tests
  #     COMMAND ${TESTRUNNER_PATH} -v --tests ${TEST_TARGET_PATH}
  #     WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endmacro()

function(ADD_CPPTEST)
  # set(options )
  set(oneValueArgs NAME PARAM)
  set(multiValueArgs SRCS LIBS)
  cmake_parse_arguments(ADD_CPPTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(TEST_LIBS RobotTestingFramework::RTF)
  if(ENABLE_PLUGIN)
    set(TEST_LIBS ${TEST_LIBS} RobotTestingFramework::RTF_dll)
  endif()
  if(ENABLE_LUA_PLUGIN)
    set(TEST_LIBS ${TEST_LIBS} RobotTestingFramework::RTF_lua)
  endif()
  if(ENABLE_PYTHON_PLUGIN)
    set(TEST_LIBS ${TEST_LIBS} RobotTestingFramework::RTF_python)
  endif()

  if(ENABLE_RUBY_PLUGIN)
    set(TEST_LIBS ${TEST_LIBS} RobotTestingFramework::RTF_ruby)
  endif()

  set(TEST_LIBS ${TEST_LIBS} ${ADD_CPPTEST_LIBS})

  add_executable(${ADD_CPPTEST_NAME} ${ADD_CPPTEST_SRCS})
  target_link_libraries(${ADD_CPPTEST_NAME} ${TEST_LIBS})
  set_target_properties(${ADD_CPPTEST_NAME}
                        PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY "${TEST_TARGET_PATH}")
  # adding test unit
  add_test(NAME ${ADD_CPPTEST_NAME}
           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
           COMMAND ${ADD_CPPTEST_NAME} ${ADD_CPPTEST_PARAM})
endfunction()


function(ADD_RobotTestingFramework_CPPTEST)
  # set(options )
  set(oneValueArgs NAME PARAM ENV)
  set(multiValueArgs SRCS LIBS)
  cmake_parse_arguments(ADD_RobotTestingFramework_CPPTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(TEST_LIBS RobotTestingFramework::RTF
                RobotTestingFramework::RTF_dll)
  if(ENABLE_LUA_PLUGIN)
    set(TEST_LIBS ${TEST_LIBS} RobotTestingFramework::RTF_lua)
  endif()
  if(ENABLE_PYTHON_PLUGIN)
    set(TEST_LIBS ${TEST_LIBS} RobotTestingFramework::RTF_python)
  endif()

  if(ENABLE_RUBY_PLUGIN)
    set(TEST_LIBS ${TEST_LIBS} RobotTestingFramework::RTF_ruby)
  endif()

  set(TEST_LIBS ${TEST_LIBS} ${ADD_RobotTestingFramework_CPPTEST_LIBS})

  add_library(${ADD_RobotTestingFramework_CPPTEST_NAME} MODULE ${ADD_RobotTestingFramework_CPPTEST_SRCS})
  target_link_libraries(${ADD_RobotTestingFramework_CPPTEST_NAME} ${TEST_LIBS})

  # adding test unit
  add_test(NAME ${ADD_RobotTestingFramework_CPPTEST_NAME}
           COMMAND ${TESTRUNNER_PATH} -v --no-output --no-summary -p ${ADD_RobotTestingFramework_CPPTEST_PARAM} -e "${ADD_RobotTestingFramework_CPPTEST_ENV}" --test $<TARGET_FILE:${ADD_RobotTestingFramework_CPPTEST_NAME}>
           WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

macro(ADD_RobotTestingFramework_TEST_SCRIPT SOURCE)
  configure_file(${SOURCE} ${TEST_TARGET_PATH}/${SOURCE} COPYONLY)
  # adding test unit
  add_test(NAME ${SOURCE}
           COMMAND ${TESTRUNNER_PATH} -v --no-output --no-summary --test ${TEST_TARGET_PATH}/${SOURCE}
           WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endmacro()

macro(ADD_RobotTestingFramework_LUATEST SOURCE)
  add_robottestingframework_test_script(${SOURCE})
endmacro()

macro(ADD_RobotTestingFramework_PYTHONTEST SOURCE)
  add_robottestingframework_test_script(${SOURCE})
endmacro()

macro(ADD_RobotTestingFramework_RUBYTEST SOURCE)
  add_robottestingframework_test_script(${SOURCE})
endmacro()
