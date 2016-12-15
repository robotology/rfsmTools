# Copyright: 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 
project(rFSM)

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
            include/rfsmUtils.h
   )

set(sources src/rfsm.cpp
            src/rfsmUtils.cpp
   )


source_group("Header Files" FILES ${headers})
source_group("Source Files" FILES ${sources})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${YARP_INCLUDE_DIRS}
                    ${LUA_INCLUDE_DIR}
                    ./include)

add_library(${PROJECT_NAME} ${headers} ${sources})

target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES} ${LUA_LIBRARY})

# Add install target
#yarp_install(TARGETS ${PROJECT_NAME}
#             COMPONENT Runtime
#             LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
#             ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR})
             
