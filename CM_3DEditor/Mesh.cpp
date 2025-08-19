#include "Mesh.h"
#include <QDebug>
#include <QVector>
#include <cmath>

struct VertexData {
    QVector3D position;
    QVector3D normal;
    QVector3D color;
    QVector2D texCoord;
};

Mesh::Mesh() : m_vertexCount(0), m_indexCount(0), m_boundingRadius(0.0f)
{
    initializeOpenGLFunctions();
    initializeBuffers();
}

Mesh::~Mesh()
{
    cleanup();
}

bool Mesh::loadFromPLY(const QString& filename)
{
    PLYLoader loader;
    if (!loader.loadPLY(filename)) {
        qDebug() << "Failed to load PLY file:" << filename;
        return false;
    }

    uploadData(loader);
    calculateBoundingBox(loader);
    
    qDebug() << "Mesh loaded successfully:" << m_vertexCount << "vertices," << m_indexCount << "indices";
    return true;
}

void Mesh::setTransform(const QMatrix4x4& transform)
{
    m_transform = transform;
}

void Mesh::render()
{
    if (!hasData()) return;

    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    m_vao.release();
}

void Mesh::renderWireframe()
{
    if (!hasData()) return;

    m_vao.bind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_vao.release();
}

void Mesh::renderPoints()
{
    if (!hasData()) return;

    m_vao.bind();
    glDrawArrays(GL_POINTS, 0, m_vertexCount);
    m_vao.release();
}

void Mesh::getBoundingBox(QVector3D& min, QVector3D& max) const
{
    min = m_boundingBoxMin;
    max = m_boundingBoxMax;
}

float Mesh::getBoundingRadius() const
{
    return m_boundingRadius;
}

QVector3D Mesh::getCenter() const
{
    return (m_boundingBoxMin + m_boundingBoxMax) * 0.5f;
}

void Mesh::initializeBuffers()
{
    // VAO 생성
    m_vao.create();
    m_vao.bind();

    // Vertex Buffer 생성
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    // Index Buffer 생성
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    // Vertex attributes 설정
    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // normal
    glEnableVertexAttribArray(2); // color
    glEnableVertexAttribArray(3); // texCoord

    m_vao.release();
}

void Mesh::uploadData(const PLYLoader& loader)
{
    const QVector<PLYVertex>& vertices = loader.getVertices();
    const QVector<PLYFace>& faces = loader.getFaces();

    m_vertexCount = vertices.size();

    // Vertex 데이터 준비
    QVector<VertexData> vertexData;
    vertexData.reserve(vertices.size());

    for (const auto& vertex : vertices) {
        VertexData vd;
        vd.position = vertex.position;
        vd.normal = vertex.normal;
        vd.color = QVector3D(vertex.color.redF(), vertex.color.greenF(), vertex.color.blueF());
        vd.texCoord = vertex.texCoord;
        vertexData.append(vd);
    }

    // Index 데이터 준비 (삼각형으로 변환)
    QVector<unsigned int> indices;
    indices.reserve(faces.size() * 3);

    for (const auto& face : faces) {
        if (face.vertexIndices.size() >= 3) {
            // 삼각형으로 변환 (fan 방식)
            for (int i = 1; i < face.vertexIndices.size() - 1; ++i) {
                indices.append(face.vertexIndices[0]);
                indices.append(face.vertexIndices[i]);
                indices.append(face.vertexIndices[i + 1]);
            }
        }
    }

    m_indexCount = indices.size();

    // VAO 바인딩
    m_vao.bind();

    // Vertex buffer 업로드
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(vertexData.constData(), vertexData.size() * sizeof(VertexData));

    // Vertex attributes 설정
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         reinterpret_cast<const void*>(offsetof(VertexData, position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         reinterpret_cast<const void*>(offsetof(VertexData, normal)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         reinterpret_cast<const void*>(offsetof(VertexData, color)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         reinterpret_cast<const void*>(offsetof(VertexData, texCoord)));

    // Index buffer 업로드
    m_indexBuffer.bind();
    m_indexBuffer.allocate(indices.constData(), indices.size() * sizeof(unsigned int));

    m_vao.release();
}

void Mesh::calculateBoundingBox(const PLYLoader& loader)
{
    loader.calculateBoundingBox(m_boundingBoxMin, m_boundingBoxMax);
    
    // 바운딩 반지름 계산
    QVector3D center = getCenter();
    float maxDistance = 0.0f;
    
    const QVector<PLYVertex>& vertices = loader.getVertices();
    for (const auto& vertex : vertices) {
        float distance = (vertex.position - center).length();
        maxDistance = qMax(maxDistance, distance);
    }
    
    m_boundingRadius = maxDistance;
}

void Mesh::cleanup()
{
    m_vao.destroy();
    m_vertexBuffer.destroy();
    m_indexBuffer.destroy();
}
