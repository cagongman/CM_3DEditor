#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_viewerWidget = new ViewerWidget(this);

    setCentralWidget(m_viewerWidget);
}

MainWindow::~MainWindow()
{
    qDebug() << "this is main windwow";
    delete ui;
}
