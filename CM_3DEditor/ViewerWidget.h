#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class ViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    ViewerWidget(QWidget* parent = nullptr);
    ~ViewerWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

#endif // VIEWERWIDGET_H
