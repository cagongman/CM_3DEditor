#ifndef PLYLOADER_H
#define PLYLOADER_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QColor>
#include <QFile>
#include <QTextStream>

struct PLYVertex {
    QVector3D position;
    QVector3D normal;
    QColor color;
    QVector2D texCoord;
};

struct PLYFace {
    QVector<int> vertexIndices;
    QColor color;
};

class PLYLoader
{
public:
    PLYLoader();
    ~PLYLoader();

    // PLY 파일 로드
    bool loadPLY(const QString& filename);
    
    // 로드된 데이터 접근
    const QVector<PLYVertex>& getVertices() const { return m_vertices; }
    const QVector<PLYFace>& getFaces() const { return m_faces; }
    
    // 메시 정보
    int getVertexCount() const { return m_vertices.size(); }
    int getFaceCount() const { return m_faces.size(); }
    
    // 바운딩 박스 계산
    void calculateBoundingBox(QVector3D& min, QVector3D& max) const;
    
    // 법선 벡터 계산
    void calculateNormals();

private:
    QVector<PLYVertex> m_vertices;
    QVector<PLYFace> m_faces;
    
    // PLY 파일 파싱 헬퍼 함수들
    bool parseHeader(QTextStream& stream);
    bool parseVertexData(QTextStream& stream, int vertexCount);
    bool parseFaceData(QTextStream& stream, int faceCount);
    
    // 데이터 타입 파싱
    float parseFloat(const QString& str);
    int parseInt(const QString& str);
    QColor parseColor(const QStringList& parts, int startIndex);
};

#endif // PLYLOADER_H
