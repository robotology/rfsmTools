#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
include(CMakePackageConfigHelpers)

find_package(Lua)
if(NOT LUA_FOUND)
    find_package(Lua53)
endif()
if(NOT LUA_FOUND)
    find_package(Lua52)
endif()
if(NOT LUA_FOUND)
    find_package(Lua51)
endif()
if(NOT LUA_FOUND)
    find_package(Lua50 REQUIRED)
endif()


set(headers include/rfsm.h
            include/rfsmUtils.h)

#########################################################################
# Control where libraries and executables are placed during the build
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# check if we need to embed rfsm lua files
if(EMBED_RFSM)
    add_definitions(" -DWITH_EMBEDDED_RFSM")
    add_executable(embedRes src/embedRes.cpp)
    add_custom_command(
      OUTPUT gen_rfsm_res.c
      COMMAND embedRes gen_rfsm_res ${CMAKE_CURRENT_SOURCE_DIR}/res/rfsm.lua
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/res/rfsm.lua)

    add_custom_command(
      OUTPUT gen_rfsm_utils_res.c
      COMMAND embedRes gen_rfsm_utils_res ${CMAKE_CURRENT_SOURCE_DIR}/res/utils.lua
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/res/utils.lua)

    set(resources res/utils.lua res/rfsm.lua)
    set(sources src/rfsm.cpp
                src/rfsmUtils.cpp
                gen_rfsm_res.c
                gen_rfsm_utils_res.c)

else()
    set(sources src/rfsm.cpp
                src/rfsmUtils.cpp)
endif()

source_group("Header Files" FILES ${headers})
source_group("Source Files" FILES ${sources})


include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                ${LUA_INCLUDE_DIR}
                ./include)

if(WIN32)
    add_library(rFSM ${headers} ${sources} ${resources})
else()
    add_library(rFSM SHARED ${headers} ${sources} ${resources})
endif()

target_include_directories(rFSM
                            PUBLIC      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>   # include folder, inside build
                                        $<INSTALL_INTERFACE:include>                              # include folder, after installation
                            )

target_link_libraries(rFSM ${LUA_LIBRARY})

add_library(rFSMTools::rFSM ALIAS rFSM)

# choose which header files should be installed
set_property(TARGET rFSM PROPERTY PUBLIC_HEADER include/rfsm.h)

install(TARGETS rFSM
        EXPORT rFSM
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION RUNTIME DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rFSM)

include(InstallBasicPackageFiles)
install_basic_package_files(rFSM
                            VERSION ${${PROJECT_NAME}_VERSION}
                            COMPATIBILITY AnyNewerVersion
                            VARS_PREFIX rFSMTools
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# Add bindings
add_subdirectory(bindings)
