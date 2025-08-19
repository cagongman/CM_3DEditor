#include "ViewerWidget.h"
#include <QDebug>
#include <QtMath>

ViewerWidget::ViewerWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_renderer(nullptr)
    , m_mesh(nullptr)
    , m_camera(nullptr)
    , m_mousePressed(false)
    , m_mouseButton(Qt::NoButton)
    , m_animationTimer(nullptr)
    , m_rotationAngle(0.0f)
{
    // 마우스 추적 활성화
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    
    // 애니메이션 타이머 설정
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &ViewerWidget::updateScene);
    m_animationTimer->start(16); // ~60 FPS
}

ViewerWidget::~ViewerWidget()
{
    delete m_renderer;
    delete m_mesh;
    delete m_camera;
}

bool ViewerWidget::loadPLYFile(const QString& filename)
{
    if (!m_mesh) {
        m_mesh = new Mesh();
    }
    
    if (m_mesh->loadFromPLY(filename)) {
        fitToView();
        update();
        return true;
    }
    
    return false;
}

void ViewerWidget::setRenderMode(Renderer::RenderMode mode)
{
    if (m_renderer) {
        m_renderer->setRenderMode(mode);
        update();
    }
}

void ViewerWidget::setShaderType(Renderer::ShaderType type)
{
    if (m_renderer) {
        m_renderer->setShaderType(type);
        update();
    }
}

void ViewerWidget::setBackgroundColor(const QColor& color)
{
    if (m_renderer) {
        m_renderer->setBackgroundColor(color);
        update();
    }
}

void ViewerWidget::setWireframeColor(const QColor& color)
{
    if (m_renderer) {
        m_renderer->setWireframeColor(color);
        update();
    }
}

void ViewerWidget::setPointSize(float size)
{
    if (m_renderer) {
        m_renderer->setPointSize(size);
        update();
    }
}

void ViewerWidget::resetCamera()
{
    if (m_camera) {
        m_camera->reset();
        update();
    }
}

void ViewerWidget::fitToView()
{
    if (!m_mesh || !m_camera) return;
    
    // 메시의 바운딩 박스 계산
    QVector3D min, max;
    m_mesh->getBoundingBox(min, max);
    
    // 메시 중심점 계산
    QVector3D center = (min + max) * 0.5f;
    float radius = m_mesh->getBoundingRadius();
    
    // 카메라 위치 설정
    float distance = radius * 2.0f;
    QVector3D cameraPos = center + QVector3D(distance, distance, distance);
    
    m_camera->setPosition(cameraPos);
    m_camera->setTarget(center);
    
    update();
}

void ViewerWidget::initializeGL()
{
    initializeOpenGLFunctions();
    
    // 렌더러 초기화
    m_renderer = new Renderer();
    m_renderer->initialize();
    
    // 카메라 초기화
    m_camera = new Camera();
    setupCamera();
    
    // 조명 설정
    setupLighting();
    
    // 렌더러에 카메라 설정
    m_renderer->setCamera(m_camera);
    
    qDebug() << "ViewerWidget initialized successfully";
}

void ViewerWidget::resizeGL(int w, int h)
{
    if (m_renderer) {
        m_renderer->resize(w, h);
    }
}

void ViewerWidget::paintGL()
{
    if (m_renderer && m_mesh) {
        m_renderer->setMesh(m_mesh);
        updateModelMatrix();
        m_renderer->render();
    }
}

void ViewerWidget::mousePressEvent(QMouseEvent* event)
{
    m_mousePressed = true;
    m_mouseButton = event->button();
    m_lastMousePos = event->pos();
    setFocus();
}

void ViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_mousePressed || !m_camera) return;
    
    QPoint delta = event->pos() - m_lastMousePos;
    
    switch (m_mouseButton) {
        case Qt::LeftButton:
            handleMouseOrbit(delta);
            break;
        case Qt::MiddleButton:
        case Qt::RightButton:
            handleMousePan(delta);
            break;
        default:
            break;
    }
    
    m_lastMousePos = event->pos();
    update();
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_mousePressed = false;
    m_mouseButton = Qt::NoButton;
}

void ViewerWidget::wheelEvent(QWheelEvent* event)
{
    if (m_camera) {
        handleMouseZoom(event->angleDelta().y());
        update();
    }
}

void ViewerWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_R:
            resetCamera();
            break;
        case Qt::Key_F:
            fitToView();
            break;
        case Qt::Key_1:
            setRenderMode(Renderer::Solid);
            break;
        case Qt::Key_2:
            setRenderMode(Renderer::Wireframe);
            break;
        case Qt::Key_3:
            setRenderMode(Renderer::Points);
            break;
        case Qt::Key_B:
            setShaderType(Renderer::Basic);
            break;
        case Qt::Key_P:
            setShaderType(Renderer::Phong);
            break;
        default:
            QOpenGLWidget::keyPressEvent(event);
            break;
    }
}

void ViewerWidget::updateScene()
{
    // 애니메이션 업데이트
    m_rotationAngle += 0.5f;
    if (m_rotationAngle >= 360.0f) {
        m_rotationAngle -= 360.0f;
    }
    
    // 자동 회전 (선택적)
    // updateModelMatrix();
    // update();
}

void ViewerWidget::setupCamera()
{
    m_camera->setPosition(QVector3D(5, 5, 5));
    m_camera->setTarget(QVector3D(0, 0, 0));
    m_camera->setUpVector(QVector3D(0, 1, 0));
    m_camera->setFOV(45.0f);
    m_camera->setAspectRatio(float(width()) / float(height()));
    m_camera->setNearPlane(0.1f);
    m_camera->setFarPlane(1000.0f);
}

void ViewerWidget::setupLighting()
{
    if (m_renderer) {
        m_renderer->setLightPosition(QVector3D(5, 5, 5));
        m_renderer->setLightColor(QColor(255, 255, 255, 255));
    }
}

void ViewerWidget::updateModelMatrix()
{
    if (!m_renderer) return;
    
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    
    // 자동 회전 (선택적)
    // modelMatrix.rotate(m_rotationAngle, QVector3D(0, 1, 0));
    
    m_renderer->setModelMatrix(modelMatrix);
}

void ViewerWidget::handleMouseOrbit(const QPoint& delta)
{
    if (m_camera) {
        m_camera->orbit(delta.x(), delta.y());
    }
}

void ViewerWidget::handleMousePan(const QPoint& delta)
{
    if (m_camera) {
        m_camera->pan(delta.x(), delta.y());
    }
}

void ViewerWidget::handleMouseZoom(int delta)
{
    if (m_camera) {
        m_camera->zoom(delta);
    }
}
