#include "PLYLoader.h"
#include <QDebug>
#include <QRegularExpression>
#include <cmath>

PLYLoader::PLYLoader()
{
}

PLYLoader::~PLYLoader()
{
}

bool PLYLoader::loadPLY(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open PLY file:" << filename;
        return false;
    }

    QTextStream stream(&file);
    
    // 헤더 파싱
    if (!parseHeader(stream)) {
        qDebug() << "Failed to parse PLY header";
        return false;
    }

    file.close();
    return true;
}

bool PLYLoader::parseHeader(QTextStream& stream)
{
    QString line = stream.readLine();
    if (line != "ply") {
        qDebug() << "Not a valid PLY file";
        return false;
    }

    line = stream.readLine();
    if (line != "format ascii 1.0") {
        qDebug() << "Only ASCII PLY format is supported";
        return false;
    }

    int vertexCount = 0;
    int faceCount = 0;
    bool hasNormals = false;
    bool hasColors = false;
    bool hasTexCoords = false;

    // 헤더 정보 파싱
    while (!stream.atEnd()) {
        line = stream.readLine().trimmed();
        
        if (line.startsWith("element vertex")) {
            vertexCount = line.split(" ").last().toInt();
        }
        else if (line.startsWith("element face")) {
            faceCount = line.split(" ").last().toInt();
        }
        else if (line.startsWith("property")) {
            QStringList parts = line.split(" ");
            if (parts.size() >= 3) {
                if (parts[2] == "nx" || parts[2] == "ny" || parts[2] == "nz") {
                    hasNormals = true;
                }
                else if (parts[2] == "red" || parts[2] == "green" || parts[2] == "blue" || parts[2] == "alpha") {
                    hasColors = true;
                }
                else if (parts[2] == "s" || parts[2] == "t") {
                    hasTexCoords = true;
                }
            }
        }
        else if (line == "end_header") {
            break;
        }
    }

    // 버텍스 데이터 파싱
    if (!parseVertexData(stream, vertexCount)) {
        return false;
    }

    // 페이스 데이터 파싱
    if (!parseFaceData(stream, faceCount)) {
        return false;
    }

    // 법선 벡터 계산
    if (!hasNormals) {
        calculateNormals();
    }

    qDebug() << "Loaded PLY file with" << vertexCount << "vertices and" << faceCount << "faces";
    return true;
}

bool PLYLoader::parseVertexData(QTextStream& stream, int vertexCount)
{
    m_vertices.clear();
    m_vertices.reserve(vertexCount);

    for (int i = 0; i < vertexCount; ++i) {
        if (stream.atEnd()) {
            qDebug() << "Unexpected end of file while parsing vertices";
            return false;
        }

        QString line = stream.readLine().trimmed();
        QStringList parts = line.split(QRegularExpression("\\s+"));
        
        if (parts.size() < 3) {
            qDebug() << "Invalid vertex data at line" << i;
            return false;
        }

        PLYVertex vertex;
        
        // Position (x, y, z)
        vertex.position.setX(parseFloat(parts[0]));
        vertex.position.setY(parseFloat(parts[1]));
        vertex.position.setZ(parseFloat(parts[2]));

        int partIndex = 3;

        // Normal (nx, ny, nz)
        if (partIndex + 2 < parts.size()) {
            vertex.normal.setX(parseFloat(parts[partIndex]));
            vertex.normal.setY(parseFloat(parts[partIndex + 1]));
            vertex.normal.setZ(parseFloat(parts[partIndex + 2]));
            partIndex += 3;
        }

        // Color (red, green, blue, alpha)
        if (partIndex + 2 < parts.size()) {
            vertex.color = parseColor(parts, partIndex);
            partIndex += 4;
        } else {
            vertex.color = QColor(128, 128, 128, 255); // 기본 회색
        }

        // Texture coordinates (s, t)
        if (partIndex + 1 < parts.size()) {
            vertex.texCoord.setX(parseFloat(parts[partIndex]));
            vertex.texCoord.setY(parseFloat(parts[partIndex + 1]));
        }

        m_vertices.append(vertex);
    }

    return true;
}

bool PLYLoader::parseFaceData(QTextStream& stream, int faceCount)
{
    m_faces.clear();
    m_faces.reserve(faceCount);

    for (int i = 0; i < faceCount; ++i) {
        if (stream.atEnd()) {
            qDebug() << "Unexpected end of file while parsing faces";
            return false;
        }

        QString line = stream.readLine().trimmed();
        QStringList parts = line.split(QRegularExpression("\\s+"));
        
        if (parts.size() < 1) {
            qDebug() << "Invalid face data at line" << i;
            return false;
        }

        PLYFace face;
        int vertexCount = parseInt(parts[0]);
        
        if (parts.size() < 1 + vertexCount) {
            qDebug() << "Insufficient face data at line" << i;
            return false;
        }

        // Vertex indices
        for (int j = 0; j < vertexCount; ++j) {
            face.vertexIndices.append(parseInt(parts[1 + j]));
        }

        // Face color (if available)
        if (parts.size() >= 1 + vertexCount + 3) {
            face.color = parseColor(parts, 1 + vertexCount);
        } else {
            face.color = QColor(128, 128, 128, 255); // 기본 회색
        }

        m_faces.append(face);
    }

    return true;
}

float PLYLoader::parseFloat(const QString& str)
{
    bool ok;
    float value = str.toFloat(&ok);
    return ok ? value : 0.0f;
}

int PLYLoader::parseInt(const QString& str)
{
    bool ok;
    int value = str.toInt(&ok);
    return ok ? value : 0;
}

QColor PLYLoader::parseColor(const QStringList& parts, int startIndex)
{
    if (startIndex + 2 >= parts.size()) {
        return QColor(128, 128, 128, 255);
    }

    int r = parseInt(parts[startIndex]);
    int g = parseInt(parts[startIndex + 1]);
    int b = parseInt(parts[startIndex + 2]);
    int a = 255;

    if (startIndex + 3 < parts.size()) {
        a = parseInt(parts[startIndex + 3]);
    }

    return QColor(r, g, b, a);
}

void PLYLoader::calculateBoundingBox(QVector3D& min, QVector3D& max) const
{
    if (m_vertices.isEmpty()) {
        min = max = QVector3D(0, 0, 0);
        return;
    }

    min = max = m_vertices[0].position;
    
    for (const auto& vertex : m_vertices) {
        min.setX(qMin(min.x(), vertex.position.x()));
        min.setY(qMin(min.y(), vertex.position.y()));
        min.setZ(qMin(min.z(), vertex.position.z()));
        
        max.setX(qMax(max.x(), vertex.position.x()));
        max.setY(qMax(max.y(), vertex.position.y()));
        max.setZ(qMax(max.z(), vertex.position.z()));
    }
}

void PLYLoader::calculateNormals()
{
    // 모든 법선 벡터를 0으로 초기화
    for (auto& vertex : m_vertices) {
        vertex.normal = QVector3D(0, 0, 0);
    }

    // 각 면의 법선 벡터를 계산하고 주변 버텍스에 더함
    for (const auto& face : m_faces) {
        if (face.vertexIndices.size() < 3) continue;

        // 면의 법선 벡터 계산
        QVector3D v0 = m_vertices[face.vertexIndices[0]].position;
        QVector3D v1 = m_vertices[face.vertexIndices[1]].position;
        QVector3D v2 = m_vertices[face.vertexIndices[2]].position;

        QVector3D edge1 = v1 - v0;
        QVector3D edge2 = v2 - v0;
        QVector3D faceNormal = QVector3D::crossProduct(edge1, edge2);
        faceNormal.normalize();

        // 각 버텍스에 법선 벡터 추가
        for (int vertexIndex : face.vertexIndices) {
            m_vertices[vertexIndex].normal += faceNormal;
        }
    }

    // 모든 법선 벡터 정규화
    for (auto& vertex : m_vertices) {
        vertex.normal.normalize();
    }
}
