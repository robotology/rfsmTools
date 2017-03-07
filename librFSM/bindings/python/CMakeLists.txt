# Copyright: 2017 iCub Facility, Istituto Italiano di Tecnologia
# Author: Silvio Traversaro
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
#

set(CMAKE_SWIG_FLAGS "-Wextra;-module;rfsm;-threads")
set(LIBRFSM_USES_PYTHON_VERSION "" CACHE STRING "Specify python version to use" )

find_package(PythonInterp ${LIBRFSM_USES_PYTHON_VERSION} REQUIRED)
if(NOT LIBRFSM_USES_PYTHON_VERSION)
  set (LIBRFSM_USES_PYTHON_VERSION ${PYTHON_VERSION_STRING})
endif()

find_package(PythonLibs ${LIBRFSM_USES_PYTHON_VERSION} EXACT)
include_directories(${PYTHON_INCLUDE_PATH})
link_libraries(${PYTHON_LIBRARY})

## in Windows it seems necessary to declare explictly the link_directory
if(WIN32)
  get_filename_component(PYTHON_DIR ${PYTHON_LIBRARY} PATH)
  link_directories(${PYTHON_DIR})
endif()

set(target_name rfsm_python)

# installation path is determined reliably on most platforms using distutils
execute_process(COMMAND ${PYTHON_EXECUTABLE}
        -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix=''))"
        OUTPUT_VARIABLE PYTHON_INSTDIR)
string(STRIP ${PYTHON_INSTDIR} PYTHON_INSTDIR)

set(CMAKE_SWIG_OUTDIR ${CMAKE_BINARY_DIR}/${PYTHON_INSTDIR})
set_source_files_properties(../rfsm.i PROPERTIES CPLUSPLUS ON)
swig_add_module(${target_name} python ../rfsm.i)
set_target_properties(_${target_name} PROPERTIES OUTPUT_NAME "_rfsm")
set_target_properties(_${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PYTHON_INSTDIR})
swig_link_libraries(${target_name} rFSM)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rfsm.py DESTINATION ${PYTHON_INSTDIR} )
install(TARGETS _${target_name} DESTINATION ${PYTHON_INSTDIR} )

if(WIN32)
  set_target_properties(_${target_name} PROPERTIES SUFFIX ".pyd")
endif(WIN32)