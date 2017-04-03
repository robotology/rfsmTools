
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


if(YARP_FOUND)
    add_definitions(-DUSE_YARP)
endif()

include_directories(../QGVCore
                    ../../librFSM/include
                    ./sourceEditor
                     ${YARP_INCLUDE_DIRS})

set(rfsmgui_qt_SRCS main.cpp
                    MainWindow.cpp
                    QGraphicsViewEc.cpp
                    sourceEditor/SourceEditorWindow.cpp
                    sourceEditor/highlighter.cpp
                    sourceEditor/codeeditor.cpp)

set(rfsmgui_qt_HDRS MainWindow.h
                    QGraphicsViewEc.h
                    QGraphicsViewEc.h
                    sourceEditor/SourceEditorWindow.h
                    sourceEditor/highlighter.h
                    sourceEditor/codeeditor.h)

set(rfsmgui_qt_QRC_FILES ress.qrc)

set(rfsmgui_qt_UI_FILES MainWindow.ui
                        sourceEditor/SourceEditorWindow.ui)


qt5_add_resources(rfsmgui_qt_QRC_GEN_SRCS ${rfsmgui_qt_QRC_FILES})
qt5_wrap_ui(rfsmgui_qt_UI_GEN_SRCS ${rfsmgui_qt_UI_FILES})

source_group("Source Files" FILES ${rfsmgui_qt_SRCS})
source_group("Header Files" FILES ${rfsmgui_qt_HDRS})
source_group("Resources Files" FILES ${rfsmgui_qt_QRC_FILES})
source_group("UI Files" FILES ${rfsmgui_qt_UI_FILES})
source_group("Generated Files" FILES ${rfsmgui_qt_QRC_GEN_SRCS}
                                  ${rfsmgui_qt_UI_GEN_SRCS})


add_executable(${PROJECTNAME} WIN32 ${rfsmgui_qt_SRCS}
                                      ${rfsmgui_qt_HDRS}
                                      ${rfsmgui_qt_QRC_FILES}
                                      ${rfsmgui_qt_QRC_GEN_SRCS}
                                      ${rfsmgui_qt_UI_GEN_SRCS}
                                      ${rfsmgui_qt_UI_FILES})



#find_package(Qt5PrintSupport REQUIRED)
target_link_libraries(${PROJECTNAME} qgvcore Qt5::Widgets Qt5::Gui rFSM ${YARP_LIBRARIES})


#if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
if(WIN32)
    target_link_libraries(${PROJECTNAME} qgvcore Qt5::WinMain rFSM ${YARP_LIBRARIES})
endif()

install(TARGETS ${PROJECTNAME}
        EXPORT ${PROJECTNAME}
        COMPONENT runtime
        RUNTIME DESTINATION bin)
       
