#include "Renderer.h"
#include <QDebug>

Renderer::Renderer()
    : m_renderMode(Solid)
    , m_shaderType(Basic)
    , m_backgroundColor(50, 50, 50, 255)
    , m_wireframeColor(255, 255, 255, 255)
    , m_pointSize(5.0f)
    , m_lightPosition(5, 5, 5)
    , m_lightColor(255, 255, 255, 255)
    , m_mesh(nullptr)
    , m_camera(nullptr)
    , m_basicShader(nullptr)
    , m_phongShader(nullptr)
    , m_wireframeShader(nullptr)
    , m_pointShader(nullptr)
    , m_customShader(nullptr)
    , m_initialized(false)
{
    m_modelMatrix.setToIdentity();
}

Renderer::~Renderer()
{
    cleanup();
}

void Renderer::initialize()
{
    if (m_initialized) return;

    initializeOpenGLFunctions();
    
    // OpenGL 설정
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 쉐이더 설정
    setupShaders();
    
    m_initialized = true;
    qDebug() << "Renderer initialized successfully";
}

void Renderer::resize(int width, int height)
{
    if (!m_initialized) return;
    
    glViewport(0, 0, width, height);
    
    if (m_camera) {
        m_camera->setAspectRatio(float(width) / float(height));
    }
}

void Renderer::setRenderMode(RenderMode mode)
{
    m_renderMode = mode;
}

void Renderer::setShaderType(ShaderType type)
{
    m_shaderType = type;
}

void Renderer::setBackgroundColor(const QColor& color)
{
    m_backgroundColor = color;
}

void Renderer::setLightPosition(const QVector3D& position)
{
    m_lightPosition = position;
}

void Renderer::setLightColor(const QColor& color)
{
    m_lightColor = color;
}

void Renderer::setWireframeColor(const QColor& color)
{
    m_wireframeColor = color;
}

void Renderer::setPointSize(float size)
{
    m_pointSize = size;
}

void Renderer::setMesh(Mesh* mesh)
{
    m_mesh = mesh;
}

void Renderer::setModelMatrix(const QMatrix4x4& matrix)
{
    m_modelMatrix = matrix;
}

void Renderer::setCamera(Camera* camera)
{
    m_camera = camera;
}

void Renderer::render()
{
    if (!m_initialized || !m_mesh || !m_camera) return;

    clear();
    renderMesh();
}

void Renderer::clear()
{
    glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), 
                 m_backgroundColor.blueF(), m_backgroundColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setCustomShader(Shader* shader)
{
    m_customShader = shader;
}

Shader* Renderer::getCurrentShader() const
{
    switch (m_shaderType) {
        case Basic:
            return m_basicShader;
        case Phong:
            return m_phongShader;
        case Custom:
            return m_customShader;
        default:
            return m_basicShader;
    }
}

void Renderer::setupShaders()
{
    // 기본 쉐이더들 생성
    m_basicShader = Shader::createBasicShader();
    m_phongShader = Shader::createPhongShader();
    m_wireframeShader = Shader::createWireframeShader();
    m_pointShader = Shader::createPointShader();
    
    if (!m_basicShader || !m_phongShader || !m_wireframeShader || !m_pointShader) {
        qDebug() << "Failed to create shaders";
    }
}

void Renderer::setupLighting(Shader* shader)
{
    if (!shader) return;
    
    shader->setVec3("lightPos", m_lightPosition);
    shader->setColor("lightColor", m_lightColor);
    
    if (m_camera) {
        shader->setVec3("viewPos", m_camera->getPosition());
    }
}

void Renderer::setupMatrices(Shader* shader)
{
    if (!shader || !m_camera) return;
    
    shader->setMat4("model", m_modelMatrix);
    shader->setMat4("view", m_camera->getViewMatrix());
    shader->setMat4("projection", m_camera->getProjectionMatrix());
}

void Renderer::renderMesh()
{
    if (!m_mesh) return;

    Shader* currentShader = nullptr;
    
    // 렌더 모드에 따른 쉐이더 선택
    switch (m_renderMode) {
        case Solid:
            currentShader = getCurrentShader();
            break;
        case Wireframe:
            currentShader = m_wireframeShader;
            break;
        case Points:
            currentShader = m_pointShader;
            break;
    }
    
    if (!currentShader) return;
    
    currentShader->use();
    
    // 행렬 및 조명 설정
    setupMatrices(currentShader);
    setupLighting(currentShader);
    
    // 렌더 모드별 설정
    switch (m_renderMode) {
        case Solid:
            // 추가 쉐이더 설정
            if (m_shaderType == Phong) {
                currentShader->setFloat("shininess", 32.0f);
            }
            m_mesh->render();
            break;
            
        case Wireframe:
            currentShader->setVec3("wireframeColor", 
                QVector3D(m_wireframeColor.redF(), m_wireframeColor.greenF(), m_wireframeColor.blueF()));
            m_mesh->renderWireframe();
            break;
            
        case Points:
            currentShader->setFloat("pointSize", m_pointSize);
            m_mesh->renderPoints();
            break;
    }
    
    currentShader->release();
}

void Renderer::cleanup()
{
    delete m_basicShader;
    delete m_phongShader;
    delete m_wireframeShader;
    delete m_pointShader;
    
    m_basicShader = nullptr;
    m_phongShader = nullptr;
    m_wireframeShader = nullptr;
    m_pointShader = nullptr;
    m_customShader = nullptr;
}
