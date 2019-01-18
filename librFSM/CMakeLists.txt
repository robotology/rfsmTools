#
# Copyright (C) 2017 iCub Facility
# Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
project(librFSM)

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

target_link_libraries(rFSM ${LUA_LIBRARY})

# choose which header files should be installed
set_property(TARGET rFSM PROPERTY PUBLIC_HEADER include/rfsm.h)

install(TARGETS rFSM
        EXPORT rFSM
        COMPONENT runtime
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

include(CMakePackageConfigHelpers)
install(EXPORT rFSM
        NAMESPACE rFSM::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rFSM)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rFSMConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/rFSMConfig.cmake
                              INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rFSM)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rFSMConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rFSM)

# Add bindings
add_subdirectory(bindings)
