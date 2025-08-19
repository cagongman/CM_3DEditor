#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>

class Camera
{
public:
    enum ProjectionType {
        Perspective,
        Orthographic
    };

    Camera();

    // 카메라 설정
    void setPosition(const QVector3D& position);
    void setTarget(const QVector3D& target);
    void setUpVector(const QVector3D& up);
    void setFOV(float fov);
    void setAspectRatio(float aspect);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);
    void setProjectionType(ProjectionType type);

    // 카메라 이동 및 회전
    void orbit(float deltaX, float deltaY);
    void pan(float deltaX, float deltaY);
    void zoom(float delta);
    void reset();

    // 행렬 가져오기
    QMatrix4x4 getViewMatrix() const;
    QMatrix4x4 getProjectionMatrix() const;
    QMatrix4x4 getViewProjectionMatrix() const;

    // 카메라 정보
    QVector3D getPosition() const { return m_position; }
    QVector3D getTarget() const { return m_target; }
    QVector3D getUpVector() const { return m_up; }
    QVector3D getForward() const { return (m_target - m_position).normalized(); }
    QVector3D getRight() const { return QVector3D::crossProduct(getForward(), m_up).normalized(); }

    // 거리 계산
    float getDistance() const { return (m_target - m_position).length(); }
    void setDistance(float distance);

private:
    // 카메라 속성
    QVector3D m_position;
    QVector3D m_target;
    QVector3D m_up;
    
    // 투영 설정
    ProjectionType m_projectionType;
    float m_fov;
    float m_aspect;
    float m_nearPlane;
    float m_farPlane;
    
    // 초기값 저장
    QVector3D m_initialPosition;
    QVector3D m_initialTarget;
    QVector3D m_initialUp;
    
    // 행렬 캐시
    mutable QMatrix4x4 m_viewMatrix;
    mutable QMatrix4x4 m_projectionMatrix;
    mutable bool m_viewMatrixDirty;
    mutable bool m_projectionMatrixDirty;
    
    // 헬퍼 함수들
    void updateViewMatrix() const;
    void updateProjectionMatrix() const;
    void invalidateViewMatrix();
    void invalidateProjectionMatrix();
};

#endif // CAMERA_H
