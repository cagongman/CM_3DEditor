cmake_minimum_required(VERSION 3.19)
project(CM_3DEditor LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Widgets
    Core
    Gui
    Widgets
    OpenGL
    OpenGLWidgets
)

qt_standard_project_setup()

qt_add_executable(CM_3DEditor
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ViewerWidget.cpp
    ViewerWidget.h
)

target_link_libraries(CM_3DEditor
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::OpenGL
        Qt::OpenGLWidgets
)

include(GNUInstallDirs)

install(TARGETS CM_3DEditor
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CM_3DEditor
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
