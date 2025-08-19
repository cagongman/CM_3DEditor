#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"

class ViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ViewerWidget(QWidget* parent = nullptr);
    ~ViewerWidget();

    // PLY 파일 로드
    bool loadPLYFile(const QString& filename);
    
    // 렌더링 설정
    void setRenderMode(Renderer::RenderMode mode);
    void setShaderType(Renderer::ShaderType type);
    void setBackgroundColor(const QColor& color);
    void setWireframeColor(const QColor& color);
    void setPointSize(float size);
    
    // 카메라 제어
    void resetCamera();
    void fitToView();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    
    // 마우스 이벤트
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
    // 키보드 이벤트
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void updateScene();

private:
    // 렌더링 시스템
    Renderer* m_renderer;
    Mesh* m_mesh;
    Camera* m_camera;
    
    // 마우스 제어
    QPoint m_lastMousePos;
    bool m_mousePressed;
    Qt::MouseButton m_mouseButton;
    
    // 애니메이션
    QTimer* m_animationTimer;
    float m_rotationAngle;
    
    // 헬퍼 함수들
    void setupCamera();
    void setupLighting();
    void updateModelMatrix();
    void handleMouseOrbit(const QPoint& delta);
    void handleMousePan(const QPoint& delta);
    void handleMouseZoom(int delta);
};

#endif // VIEWERWIDGET_H
