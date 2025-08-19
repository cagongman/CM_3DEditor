#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <QVector2D>
#include <QColor>
#include <QMatrix4x4>
#include "PLYLoader.h"

class Mesh : protected QOpenGLFunctions
{
public:
    Mesh();
    ~Mesh();

    // 메시 로드 및 설정
    bool loadFromPLY(const QString& filename);
    void setTransform(const QMatrix4x4& transform);
    
    // 렌더링
    void render();
    void renderWireframe();
    void renderPoints();
    
    // 메시 정보
    int getVertexCount() const { return m_vertexCount; }
    int getIndexCount() const { return m_indexCount; }
    bool hasData() const { return m_vertexCount > 0; }
    
    // 바운딩 박스
    void getBoundingBox(QVector3D& min, QVector3D& max) const;
    float getBoundingRadius() const;
    
    // 메시 중심점
    QVector3D getCenter() const;

private:
    // OpenGL 버퍼들
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
    
    // 메시 데이터
    int m_vertexCount;
    int m_indexCount;
    
    // 변환 행렬
    QMatrix4x4 m_transform;
    
    // 바운딩 박스
    QVector3D m_boundingBoxMin;
    QVector3D m_boundingBoxMax;
    float m_boundingRadius;
    
    // 초기화 함수들
    void initializeBuffers();
    void uploadData(const PLYLoader& loader);
    void calculateBoundingBox(const PLYLoader& loader);
    
    // 버퍼 정리
    void cleanup();
};

#endif // MESH_H
