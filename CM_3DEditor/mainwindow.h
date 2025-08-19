#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QDockWidget>
#include "ViewerWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 파일 메뉴
    void openPLYFile();
    void saveScreenshot();
    void exit();
    
    // 뷰 메뉴
    void resetCamera();
    void fitToView();
    
    // 렌더링 설정
    void setRenderMode(int mode);
    void setShaderType(int type);
    void setBackgroundColor();
    void setWireframeColor();
    void setPointSize(int size);
    
    // 상태 업데이트
    void updateStatusBar();

private:
    Ui::MainWindow *ui;
    ViewerWidget* m_viewerWidget;
    
    // UI 컴포넌트들
    QMenu* m_fileMenu;
    QMenu* m_viewMenu;
    QMenu* m_renderMenu;
    QMenu* m_helpMenu;
    
    QToolBar* m_mainToolBar;
    
    QWidget* m_controlPanel;
    QGroupBox* m_renderGroup;
    QGroupBox* m_cameraGroup;
    QGroupBox* m_lightingGroup;
    
    QComboBox* m_renderModeCombo;
    QComboBox* m_shaderTypeCombo;
    QSlider* m_pointSizeSlider;
    QPushButton* m_bgColorButton;
    QPushButton* m_wireframeColorButton;
    
    QProgressBar* m_statusProgress;
    QLabel* m_statusLabel;
    
    // 헬퍼 함수들
    void createMenus();
    void createToolBar();
    void createControlPanel();
    void createStatusBar();
    void setupConnections();
    void updateUI();
};

#endif // MAINWINDOW_H
