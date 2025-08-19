#include "Camera.h"
#include <QtMath>

Camera::Camera()
    : m_position(0, 0, 5)
    , m_target(0, 0, 0)
    , m_up(0, 1, 0)
    , m_projectionType(Perspective)
    , m_fov(45.0f)
    , m_aspect(1.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f)
    , m_viewMatrixDirty(true)
    , m_projectionMatrixDirty(true)
{
    // 초기값 저장
    m_initialPosition = m_position;
    m_initialTarget = m_target;
    m_initialUp = m_up;
}

void Camera::setPosition(const QVector3D& position)
{
    m_position = position;
    invalidateViewMatrix();
}

void Camera::setTarget(const QVector3D& target)
{
    m_target = target;
    invalidateViewMatrix();
}

void Camera::setUpVector(const QVector3D& up)
{
    m_up = up.normalized();
    invalidateViewMatrix();
}

void Camera::setFOV(float fov)
{
    m_fov = fov;
    invalidateProjectionMatrix();
}

void Camera::setAspectRatio(float aspect)
{
    m_aspect = aspect;
    invalidateProjectionMatrix();
}

void Camera::setNearPlane(float nearPlane)
{
    m_nearPlane = nearPlane;
    invalidateProjectionMatrix();
}

void Camera::setFarPlane(float farPlane)
{
    m_farPlane = farPlane;
    invalidateProjectionMatrix();
}

void Camera::setProjectionType(ProjectionType type)
{
    m_projectionType = type;
    invalidateProjectionMatrix();
}

void Camera::orbit(float deltaX, float deltaY)
{
    float distance = getDistance();
    
    // 스피어 좌표계로 변환
    QVector3D direction = (m_position - m_target).normalized();
    float theta = qAtan2(direction.x(), direction.z());
    float phi = qAcos(direction.y());
    
    // 각도 업데이트
    theta -= deltaX * 0.01f;
    phi += deltaY * 0.01f;
    
    // phi 범위 제한
    phi = qBound(0.1f, phi, float(M_PI) - 0.1f);
    
    // 새로운 위치 계산
    float x = distance * qSin(phi) * qSin(theta);
    float y = distance * qCos(phi);
    float z = distance * qSin(phi) * qCos(theta);
    
    m_position = m_target + QVector3D(x, y, z);
    invalidateViewMatrix();
}

void Camera::pan(float deltaX, float deltaY)
{
    float distance = getDistance();
    float panSpeed = distance * 0.001f;
    
    QVector3D right = getRight();
    QVector3D up = m_up;
    
    QVector3D offset = right * (-deltaX * panSpeed) + up * (deltaY * panSpeed);
    
    m_position += offset;
    m_target += offset;
    invalidateViewMatrix();
}

void Camera::zoom(float delta)
{
    float distance = getDistance();
    float zoomSpeed = 0.1f;
    
    // 거리 업데이트
    distance *= (1.0f - delta * zoomSpeed);
    distance = qMax(0.1f, distance);
    
    setDistance(distance);
}

void Camera::reset()
{
    m_position = m_initialPosition;
    m_target = m_initialTarget;
    m_up = m_initialUp;
    invalidateViewMatrix();
}

QMatrix4x4 Camera::getViewMatrix() const
{
    if (m_viewMatrixDirty) {
        updateViewMatrix();
    }
    return m_viewMatrix;
}

QMatrix4x4 Camera::getProjectionMatrix() const
{
    if (m_projectionMatrixDirty) {
        updateProjectionMatrix();
    }
    return m_projectionMatrix;
}

QMatrix4x4 Camera::getViewProjectionMatrix() const
{
    return getProjectionMatrix() * getViewMatrix();
}

void Camera::setDistance(float distance)
{
    QVector3D direction = (m_position - m_target).normalized();
    m_position = m_target + direction * distance;
    invalidateViewMatrix();
}

void Camera::updateViewMatrix() const
{
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(m_position, m_target, m_up);
    m_viewMatrixDirty = false;
}

void Camera::updateProjectionMatrix() const
{
    m_projectionMatrix.setToIdentity();
    
    if (m_projectionType == Perspective) {
        m_projectionMatrix.perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
    } else {
        float halfHeight = m_farPlane * qTan(qDegreesToRadians(m_fov * 0.5f));
        float halfWidth = halfHeight * m_aspect;
        m_projectionMatrix.ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
    }
    
    m_projectionMatrixDirty = false;
}

void Camera::invalidateViewMatrix()
{
    m_viewMatrixDirty = true;
}

void Camera::invalidateProjectionMatrix()
{
    m_projectionMatrixDirty = true;
}
