#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_viewerWidget(nullptr)
    , m_fileMenu(nullptr)
    , m_viewMenu(nullptr)
    , m_renderMenu(nullptr)
    , m_helpMenu(nullptr)
    , m_mainToolBar(nullptr)
    , m_controlPanel(nullptr)
    , m_renderGroup(nullptr)
    , m_cameraGroup(nullptr)
    , m_lightingGroup(nullptr)
    , m_renderModeCombo(nullptr)
    , m_shaderTypeCombo(nullptr)
    , m_pointSizeSlider(nullptr)
    , m_bgColorButton(nullptr)
    , m_wireframeColorButton(nullptr)
    , m_statusProgress(nullptr)
    , m_statusLabel(nullptr)
{
    ui->setupUi(this);
    
    // 윈도우 설정
    setWindowTitle("CM 3D Editor");
    resize(1200, 800);
    
    // UI 생성
    createMenus();
    createToolBar();
    createControlPanel();
    createStatusBar();
    
    // 뷰어 위젯 생성
    m_viewerWidget = new ViewerWidget(this);
    setCentralWidget(m_viewerWidget);
    
    // 연결 설정
    setupConnections();
    
    // 초기 상태 설정
    updateUI();
    
    qDebug() << "MainWindow initialized successfully";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openPLYFile()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Open PLY File",
        QString(),
        "PLY Files (*.ply);;All Files (*)"
    );
    
    if (!filename.isEmpty()) {
        if (m_viewerWidget->loadPLYFile(filename)) {
            statusBar()->showMessage("PLY file loaded successfully: " + filename, 3000);
            updateStatusBar();
        } else {
            QMessageBox::critical(this, "Error", "Failed to load PLY file: " + filename);
        }
    }
}

void MainWindow::saveScreenshot()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Save Screenshot",
        QString(),
        "PNG Files (*.png);;JPEG Files (*.jpg);;All Files (*)"
    );
    
    if (!filename.isEmpty()) {
        QPixmap screenshot = m_viewerWidget->grab();
        if (screenshot.save(filename)) {
            statusBar()->showMessage("Screenshot saved: " + filename, 3000);
        } else {
            QMessageBox::critical(this, "Error", "Failed to save screenshot");
        }
    }
}

void MainWindow::exit()
{
    QApplication::quit();
}

void MainWindow::resetCamera()
{
    m_viewerWidget->resetCamera();
    statusBar()->showMessage("Camera reset", 2000);
}

void MainWindow::fitToView()
{
    m_viewerWidget->fitToView();
    statusBar()->showMessage("Fit to view", 2000);
}

void MainWindow::setRenderMode(int mode)
{
    m_viewerWidget->setRenderMode(static_cast<Renderer::RenderMode>(mode));
    statusBar()->showMessage("Render mode changed", 1000);
}

void MainWindow::setShaderType(int type)
{
    m_viewerWidget->setShaderType(static_cast<Renderer::ShaderType>(type));
    statusBar()->showMessage("Shader type changed", 1000);
}

void MainWindow::setBackgroundColor()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Select Background Color");
    if (color.isValid()) {
        m_viewerWidget->setBackgroundColor(color);
        m_bgColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        statusBar()->showMessage("Background color changed", 1000);
    }
}

void MainWindow::setWireframeColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Select Wireframe Color");
    if (color.isValid()) {
        m_viewerWidget->setWireframeColor(color);
        m_wireframeColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        statusBar()->showMessage("Wireframe color changed", 1000);
    }
}

void MainWindow::setPointSize(int size)
{
    m_viewerWidget->setPointSize(size);
    statusBar()->showMessage(QString("Point size: %1").arg(size), 1000);
}

void MainWindow::updateStatusBar()
{
    // 메시 정보 업데이트 (나중에 구현)
    m_statusLabel->setText("Ready");
}

void MainWindow::createMenus()
{
    // 파일 메뉴
    m_fileMenu = menuBar()->addMenu("&File");
    
    QAction* openAction = new QAction("&Open PLY...", this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openPLYFile);
    m_fileMenu->addAction(openAction);
    
    QAction* saveAction = new QAction("&Save Screenshot...", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveScreenshot);
    m_fileMenu->addAction(saveAction);
    
    m_fileMenu->addSeparator();
    
    QAction* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exit);
    m_fileMenu->addAction(exitAction);
    
    // 뷰 메뉴
    m_viewMenu = menuBar()->addMenu("&View");
    
    QAction* resetAction = new QAction("&Reset Camera", this);
    resetAction->setShortcut(QKeySequence("R"));
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetCamera);
    m_viewMenu->addAction(resetAction);
    
    QAction* fitAction = new QAction("&Fit to View", this);
    fitAction->setShortcut(QKeySequence("F"));
    connect(fitAction, &QAction::triggered, this, &MainWindow::fitToView);
    m_viewMenu->addAction(fitAction);
    
    // 렌더 메뉴
    m_renderMenu = menuBar()->addMenu("&Render");
    
    QAction* solidAction = new QAction("&Solid", this);
    solidAction->setShortcut(QKeySequence("1"));
    connect(solidAction, &QAction::triggered, [this]() { setRenderMode(0); });
    m_renderMenu->addAction(solidAction);
    
    QAction* wireframeAction = new QAction("&Wireframe", this);
    wireframeAction->setShortcut(QKeySequence("2"));
    connect(wireframeAction, &QAction::triggered, [this]() { setRenderMode(1); });
    m_renderMenu->addAction(wireframeAction);
    
    QAction* pointsAction = new QAction("&Points", this);
    pointsAction->setShortcut(QKeySequence("3"));
    connect(pointsAction, &QAction::triggered, [this]() { setRenderMode(2); });
    m_renderMenu->addAction(pointsAction);
    
    // 도움말 메뉴
    m_helpMenu = menuBar()->addMenu("&Help");
    
    QAction* aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About CM 3D Editor", 
                          "CM 3D Editor\n\nA 3D model viewer and editor built with Qt and OpenGL.\n\n"
                          "Features:\n"
                          "- PLY file loading\n"
                          "- Multiple rendering modes\n"
                          "- Interactive camera controls\n"
                          "- Shader support\n\n"
                          "Controls:\n"
                          "- Left mouse: Orbit camera\n"
                          "- Right mouse: Pan camera\n"
                          "- Mouse wheel: Zoom\n"
                          "- R: Reset camera\n"
                          "- F: Fit to view\n"
                          "- 1/2/3: Render modes\n"
                          "- B/P: Shader types");
    });
    m_helpMenu->addAction(aboutAction);
}

void MainWindow::createToolBar()
{
    m_mainToolBar = addToolBar("Main");
    m_mainToolBar->setMovable(false);
    
    // 파일 액션들
    m_mainToolBar->addAction(m_fileMenu->actions().first()); // Open
    m_mainToolBar->addSeparator();
    
    // 뷰 액션들
    m_mainToolBar->addAction(m_viewMenu->actions().first()); // Reset Camera
    m_mainToolBar->addAction(m_viewMenu->actions().last());  // Fit to View
}

void MainWindow::createControlPanel()
{
    m_controlPanel = new QWidget(this);
    m_controlPanel->setMaximumWidth(250);
    m_controlPanel->setMinimumWidth(200);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(m_controlPanel);
    
    // 렌더링 그룹
    m_renderGroup = new QGroupBox("Rendering", m_controlPanel);
    QVBoxLayout* renderLayout = new QVBoxLayout(m_renderGroup);
    
    // 렌더 모드
    QLabel* renderModeLabel = new QLabel("Render Mode:", m_renderGroup);
    m_renderModeCombo = new QComboBox(m_renderGroup);
    m_renderModeCombo->addItems({"Solid", "Wireframe", "Points"});
    renderLayout->addWidget(renderModeLabel);
    renderLayout->addWidget(m_renderModeCombo);
    
    // 쉐이더 타입
    QLabel* shaderLabel = new QLabel("Shader Type:", m_renderGroup);
    m_shaderTypeCombo = new QComboBox(m_renderGroup);
    m_shaderTypeCombo->addItems({"Basic", "Phong"});
    renderLayout->addWidget(shaderLabel);
    renderLayout->addWidget(m_shaderTypeCombo);
    
    // 포인트 크기
    QLabel* pointSizeLabel = new QLabel("Point Size:", m_renderGroup);
    m_pointSizeSlider = new QSlider(Qt::Horizontal, m_renderGroup);
    m_pointSizeSlider->setRange(1, 20);
    m_pointSizeSlider->setValue(5);
    renderLayout->addWidget(pointSizeLabel);
    renderLayout->addWidget(m_pointSizeSlider);
    
    // 색상 버튼들
    m_bgColorButton = new QPushButton("Background Color", m_renderGroup);
    m_bgColorButton->setStyleSheet("background-color: #323232");
    renderLayout->addWidget(m_bgColorButton);
    
    m_wireframeColorButton = new QPushButton("Wireframe Color", m_renderGroup);
    m_wireframeColorButton->setStyleSheet("background-color: white");
    renderLayout->addWidget(m_wireframeColorButton);
    
    mainLayout->addWidget(m_renderGroup);
    
    // 카메라 그룹
    m_cameraGroup = new QGroupBox("Camera", m_controlPanel);
    QVBoxLayout* cameraLayout = new QVBoxLayout(m_cameraGroup);
    
    QPushButton* resetButton = new QPushButton("Reset Camera", m_cameraGroup);
    QPushButton* fitButton = new QPushButton("Fit to View", m_cameraGroup);
    
    cameraLayout->addWidget(resetButton);
    cameraLayout->addWidget(fitButton);
    
    mainLayout->addWidget(m_cameraGroup);
    
    // 조명 그룹
    m_lightingGroup = new QGroupBox("Lighting", m_controlPanel);
    QVBoxLayout* lightingLayout = new QVBoxLayout(m_lightingGroup);
    
    QLabel* lightingInfo = new QLabel("Lighting controls\nwill be added here", m_lightingGroup);
    lightingInfo->setAlignment(Qt::AlignCenter);
    lightingLayout->addWidget(lightingInfo);
    
    mainLayout->addWidget(m_lightingGroup);
    
    mainLayout->addStretch();
    
    // 컨트롤 패널을 도킹 위젯으로 추가
    QDockWidget* dockWidget = new QDockWidget("Controls", this);
    dockWidget->setWidget(m_controlPanel);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);
}

void MainWindow::createStatusBar()
{
    m_statusProgress = new QProgressBar(this);
    m_statusProgress->setVisible(false);
    m_statusProgress->setMaximumWidth(200);
    
    m_statusLabel = new QLabel("Ready", this);
    
    statusBar()->addWidget(m_statusLabel);
    statusBar()->addPermanentWidget(m_statusProgress);
}

void MainWindow::setupConnections()
{
    // 렌더링 설정 연결
    connect(m_renderModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::setRenderMode);
    connect(m_shaderTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::setShaderType);
    connect(m_pointSizeSlider, &QSlider::valueChanged,
            this, &MainWindow::setPointSize);
    
    // 색상 버튼 연결
    connect(m_bgColorButton, &QPushButton::clicked,
            this, &MainWindow::setBackgroundColor);
    connect(m_wireframeColorButton, &QPushButton::clicked,
            this, &MainWindow::setWireframeColor);
}

void MainWindow::updateUI()
{
    // UI 상태 업데이트 (필요시 구현)
}
