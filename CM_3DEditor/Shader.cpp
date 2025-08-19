#include "Shader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

Shader::Shader()
{
}

Shader::~Shader()
{
}

bool Shader::loadFromFiles(const QString& vertexFile, const QString& fragmentFile)
{
    QString vertexSource = readFile(vertexFile);
    QString fragmentSource = readFile(fragmentFile);
    
    if (vertexSource.isEmpty() || fragmentSource.isEmpty()) {
        return false;
    }
    
    return loadFromSource(vertexSource, fragmentSource);
}

bool Shader::loadFromSource(const QString& vertexSource, const QString& fragmentSource)
{
    // 기존 쉐이더 정리
    removeAllShaders();
    
    // 버텍스 쉐이더 컴파일
    if (!compileShader(QOpenGLShader::Vertex, vertexSource)) {
        qDebug() << "Failed to compile vertex shader";
        return false;
    }
    
    // 프래그먼트 쉐이더 컴파일
    if (!compileShader(QOpenGLShader::Fragment, fragmentSource)) {
        qDebug() << "Failed to compile fragment shader";
        return false;
    }
    
    // 쉐이더 프로그램 링크
    if (!link()) {
        qDebug() << "Failed to link shader program:" << log();
        return false;
    }
    
    return true;
}

void Shader::use()
{
    bind();
}

void Shader::release()
{
    release();
}

void Shader::setBool(const QString& name, bool value)
{
    setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setInt(const QString& name, int value)
{
    setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setFloat(const QString& name, float value)
{
    setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setVec2(const QString& name, const QVector2D& value)
{
    setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setVec3(const QString& name, const QVector3D& value)
{
    setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setVec4(const QString& name, const QVector4D& value)
{
    setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setMat4(const QString& name, const QMatrix4x4& value)
{
    setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setColor(const QString& name, const QColor& value)
{
    setUniformValue(name.toStdString().c_str(), 
                   QVector3D(value.redF(), value.greenF(), value.blueF()));
}

bool Shader::compileShader(QOpenGLShader::ShaderType type, const QString& source)
{
    QOpenGLShader* shader = new QOpenGLShader(type, this);
    if (!shader->compileSourceCode(source)) {
        qDebug() << "Shader compilation failed:" << shader->log();
        delete shader;
        return false;
    }
    
    if (!addShader(shader)) {
        qDebug() << "Failed to add shader to program";
        delete shader;
        return false;
    }
    
    return true;
}

QString Shader::readFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open shader file:" << filename;
        return QString();
    }
    
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    
    return content;
}

Shader* Shader::createBasicShader()
{
    Shader* shader = new Shader();
    if (!shader->loadFromSource(getBasicVertexShaderSource(), getBasicFragmentShaderSource())) {
        delete shader;
        return nullptr;
    }
    return shader;
}

Shader* Shader::createPhongShader()
{
    Shader* shader = new Shader();
    if (!shader->loadFromSource(getPhongVertexShaderSource(), getPhongFragmentShaderSource())) {
        delete shader;
        return nullptr;
    }
    return shader;
}

Shader* Shader::createWireframeShader()
{
    Shader* shader = new Shader();
    if (!shader->loadFromSource(getWireframeVertexShaderSource(), getWireframeFragmentShaderSource())) {
        delete shader;
        return nullptr;
    }
    return shader;
}

Shader* Shader::createPointShader()
{
    Shader* shader = new Shader();
    if (!shader->loadFromSource(getPointVertexShaderSource(), getPointFragmentShaderSource())) {
        delete shader;
        return nullptr;
    }
    return shader;
}

QString Shader::getBasicVertexShaderSource()
{
    return R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoord;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Color = aColor;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";
}

QString Shader::getBasicFragmentShaderSource()
{
    return R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
}
)";
}

QString Shader::getPhongVertexShaderSource()
{
    return R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoord;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Color = aColor;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";
}

QString Shader::getPhongFragmentShaderSource()
{
    return R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float shininess;

void main()
{
    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
}
)";
}

QString Shader::getWireframeVertexShaderSource()
{
    return R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";
}

QString Shader::getWireframeFragmentShaderSource()
{
    return R"(
#version 330 core
out vec4 FragColor;

uniform vec3 wireframeColor;

void main()
{
    FragColor = vec4(wireframeColor, 1.0);
}
)";
}

QString Shader::getPointVertexShaderSource()
{
    return R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float pointSize;

out vec3 Color;

void main()
{
    Color = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = pointSize;
}
)";
}

QString Shader::getPointFragmentShaderSource()
{
    return R"(
#version 330 core
out vec4 FragColor;

in vec3 Color;

void main()
{
    FragColor = vec4(Color, 1.0);
}
)";
}
