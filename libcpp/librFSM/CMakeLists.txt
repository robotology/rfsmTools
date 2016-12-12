# Copyright: 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 
project(rFSM)

find_package(Lua51 REQUIRED)

set(headers include/rFSM.h
   )

set(sources src/rFSM.cpp
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
             
