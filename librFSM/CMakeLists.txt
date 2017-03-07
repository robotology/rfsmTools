# Copyright: 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 
project(librFSM)

find_package(Lua)
if(NOT LUA_FOUND)
    find_package(Lua53)
elseif(NOT LUA_FOUND)
    find_package(Lua52)
elseif(NOT LUA_FOUND)
    find_package(Lua51)
elseif(NOT LUA_FOUND)
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

# Add bindings
add_subdirectory(bindings)

