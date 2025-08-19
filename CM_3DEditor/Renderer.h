#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector3D>
#include <QColor>
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

class Renderer : protected QOpenGLFunctions
{
public:
    enum RenderMode {
        Solid,
        Wireframe,
        Points
    };

    enum ShaderType {
        Basic,
        Phong,
        Custom
    };

    Renderer();
    ~Renderer();

    // 초기화
    void initialize();
    void resize(int width, int height);
    
    // 렌더링 설정
    void setRenderMode(RenderMode mode);
    void setShaderType(ShaderType type);
    void setBackgroundColor(const QColor& color);
    void setLightPosition(const QVector3D& position);
    void setLightColor(const QColor& color);
    void setWireframeColor(const QColor& color);
    void setPointSize(float size);
    
    // 메시 관리
    void setMesh(Mesh* mesh);
    void setModelMatrix(const QMatrix4x4& matrix);
    
    // 카메라 설정
    void setCamera(Camera* camera);
    
    // 렌더링
    void render();
    void clear();
    
    // 쉐이더 관리
    void setCustomShader(Shader* shader);
    Shader* getCurrentShader() const;

private:
    // 렌더링 상태
    RenderMode m_renderMode;
    ShaderType m_shaderType;
    QColor m_backgroundColor;
    QColor m_wireframeColor;
    float m_pointSize;
    
    // 조명 설정
    QVector3D m_lightPosition;
    QColor m_lightColor;
    
    // 객체들
    Mesh* m_mesh;
    Camera* m_camera;
    QMatrix4x4 m_modelMatrix;
    
    // 쉐이더들
    Shader* m_basicShader;
    Shader* m_phongShader;
    Shader* m_wireframeShader;
    Shader* m_pointShader;
    Shader* m_customShader;
    
    // OpenGL 상태
    bool m_initialized;
    
    // 헬퍼 함수들
    void setupShaders();
    void setupLighting(Shader* shader);
    void setupMatrices(Shader* shader);
    void renderMesh();
    void cleanup();
};

#endif // RENDERER_H
