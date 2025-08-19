#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QString>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>
#include <QColor>

class Shader : public QOpenGLShaderProgram
{
public:
    Shader();
    ~Shader();

    // 쉐이더 로드 및 컴파일
    bool loadFromFiles(const QString& vertexFile, const QString& fragmentFile);
    bool loadFromSource(const QString& vertexSource, const QString& fragmentSource);
    
    // 쉐이더 사용
    void use();
    void release();
    
    // Uniform 변수 설정
    void setBool(const QString& name, bool value);
    void setInt(const QString& name, int value);
    void setFloat(const QString& name, float value);
    void setVec2(const QString& name, const QVector2D& value);
    void setVec3(const QString& name, const QVector3D& value);
    void setVec4(const QString& name, const QVector4D& value);
    void setMat4(const QString& name, const QMatrix4x4& value);
    void setColor(const QString& name, const QColor& value);
    
    // 기본 쉐이더들
    static Shader* createBasicShader();
    static Shader* createPhongShader();
    static Shader* createWireframeShader();
    static Shader* createPointShader();

private:
    // 쉐이더 컴파일 헬퍼
    bool compileShader(QOpenGLShader::ShaderType type, const QString& source);
    QString readFile(const QString& filename);
    
    // 기본 쉐이더 소스 코드
    static QString getBasicVertexShaderSource();
    static QString getBasicFragmentShaderSource();
    static QString getPhongVertexShaderSource();
    static QString getPhongFragmentShaderSource();
    static QString getWireframeVertexShaderSource();
    static QString getWireframeFragmentShaderSource();
    static QString getPointVertexShaderSource();
    static QString getPointFragmentShaderSource();
};

#endif // SHADER_H
