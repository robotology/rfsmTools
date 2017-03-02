
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../QGVCore
                    ../../librFSM/include)


set(yarpviz_qt_SRCS main.cpp MainWindow.cpp QGraphicsViewEc.cpp)

set(yarpviz_qt_HDRS MainWindow.h QGraphicsViewEc.h QGraphicsViewEc.h)

set(yarpviz_qt_QRC_FILES ress.qrc)

set(yarpviz_qt_UI_FILES MainWindow.ui)


qt5_add_resources(yarpviz_qt_QRC_GEN_SRCS ${yarpviz_qt_QRC_FILES})
qt5_wrap_ui(yarpviz_qt_UI_GEN_SRCS ${yarpviz_qt_UI_FILES})

source_group("Source Files" FILES ${yarpviz_qt_SRCS})
source_group("Header Files" FILES ${yarpviz_qt_HDRS})
source_group("Resources Files" FILES ${yarpviz_qt_QRC_FILES})
source_group("UI Files" FILES ${yarpviz_qt_UI_FILES})
source_group("Generated Files" FILES ${yarpviz_qt_QRC_GEN_SRCS}
                                  ${yarpviz_qt_UI_GEN_SRCS})


add_executable(${PROJECTNAME} WIN32 ${yarpviz_qt_SRCS}
                                      ${yarpviz_qt_HDRS}
                                      ${yarpviz_qt_QRC_FILES}
                                      ${yarpviz_qt_QRC_GEN_SRCS}
                                      ${yarpviz_qt_UI_GEN_SRCS}
                                      ${yarpviz_qt_UI_FILES})



#find_package(Qt5PrintSupport REQUIRED)
target_link_libraries(${PROJECTNAME} qgvcore Qt5::Widgets Qt5::Gui rFSM)


#if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
if(WIN32)
    target_link_libraries(${PROJECTNAME} qgvcore Qt5::WinMain rFSM)
endif()

#install(TARGETS ${PROJECTNAME} COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})


