#include "ViewerWidget.h"

ViewerWidget::ViewerWidget(QWidget* parent)
{

}

ViewerWidget::~ViewerWidget()
{

}

void ViewerWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    qDebug() << "OpenGL Initialized: " << (const char*) glGetString(GL_VERSION);
}

void ViewerWidget::resizeGL(int w, int h)
{
    // 위젯의 크기가 변경될 때마다 호출됩니다.
    // 뷰포트 크기를 위젯 크기에 맞게 설정합니다.
    glViewport(0, 0, w, h);
}

void ViewerWidget::paintGL()
{
    // 렌더링 전에 색상 버퍼와 깊이 버퍼를 초기화합니다.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- 여기에 3D 모델을 그리는 코드가 들어갑니다 ---
    // (지금은 배경색으로 화면을 지우는 것만 수행합니다)
}
