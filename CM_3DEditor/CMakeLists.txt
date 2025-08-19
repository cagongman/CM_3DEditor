cmake_minimum_required(VERSION 3.19)
project(CM_3DEditor LANGUAGES CXX)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Widgets
    Core
    Gui
    Widgets
    OpenGL
    OpenGLWidgets
)

# OpenGL 라이브러리 찾기
find_package(OpenGL REQUIRED)

qt_standard_project_setup()

# 소스 파일들
set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ViewerWidget.cpp
    ViewerWidget.h
    PLYLoader.cpp
    PLYLoader.h
    Mesh.cpp
    Mesh.h
    Camera.cpp
    Camera.h
    Shader.cpp
    Shader.h
    Renderer.cpp
    Renderer.h
)

qt_add_executable(CM_3DEditor
    WIN32 MACOSX_BUNDLE
    ${SOURCES}
)

target_link_libraries(CM_3DEditor
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::OpenGL
        Qt::OpenGLWidgets
        OpenGL::GL
)

# 컴파일러 플래그 설정
if(MSVC)
    target_compile_options(CM_3DEditor PRIVATE /W4)
else()
    target_compile_options(CM_3DEditor PRIVATE -Wall -Wextra)
endif()

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
