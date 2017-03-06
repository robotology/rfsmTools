# Copyright: 2017 iCub Facility, Istituto Italiano di Tecnologia
# Author: Silvio Traversaro
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
#

########################################################################
## Options for compiling supported languages.  There's nothing magical
## about this list, any SWIG-supported language should work.

option(LIBRFSM_USES_PYTHON "Do you want to create the Python interface" FALSE)
option(LIBRFSM_USES_MATLAB "Do you want to create the MATLAB Interface" FALSE)
option(LIBRFSM_GENERATE_MATLAB "Enable if you have the experimental version of SWIG necessary for generating the Matlab wrapper" FALSE)

# We use two strategies to generate bindings: for Python we expect that the user
# has a SWIG installed on its system, while for matlab we expect that the mantainer
# that modifies the interface of the rfsm.h file, it needs to regenerate the bindings
# and commit it (this is necessary because the matlab support is not integrated
# in the master branch of SWIG).
#
# The SWIG necessary to generate the matlab bindings is:
# repo   - https://github.com/jaeandersson/swig
# commit - https://github.com/jaeandersson/swig/commit/5d3770e452b71265162001c2249dea7961660629
#
# See https://github.com/robotology/idyntree#matlab-bindings-modifications for more info


find_package(SWIG)

if(SWIG_FOUND OR LIBRFSM_USES_MATLAB)
  include(UseSWIG)

  set_source_files_properties(rfsm.i PROPERTIES CPLUSPLUS ON)

  if(LIBRFSM_USES_PYTHON)
    add_subdirectory(python)
  endif()

  if(LIBRFSM_USES_MATLAB OR LIBRFSM_GENERATE_MATLAB)
    add_subdirectory(matlab)
  endif()

endif()

if(LIBRFSM_USES_PYTHON OR LIBRFSM_GENERATE_MATLAB)
  if(NOT SWIG_FOUND)
    MESSAGE(FATAL_ERROR "Swig not found, impossible to compile or generate bindings.")
  endif()
endif()