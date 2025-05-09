#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QMatrix4x4>
#include <QTime>
#include "projectileManager.h"
#include <QOpenGLTexture>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget();

protected:
    // QOpenGLWidget methods to override
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    // Drawing methods
    void drawCorridor();
    void drawCannon();
    void drawCylindricalGrid();
    void setupLight();
    void drawAxes();

    void drawTestObject();

    // Timer for animations
    QTimer *timer;

    // Variables for projection and view
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;

    // Constants for dimensions
    const float corridorLength = 20.0f;
    const float corridorWidth = 5.0f;
    const float corridorHeight = 5.0f;
    const float gridRadius = 3.5f;
    const float gridAngle = 60.0f; // in degrees

    // Projectile manager
    ProjectileManager m_projectileManager;

    // For calculating elapsed time between frames
    QTime m_lastFrameTime;
};

#endif // MYGLWIDGET_H
