#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QMatrix4x4>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget();

protected:
    // Méthodes de QOpenGLWidget à surcharger
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    // Méthodes de dessin
    void drawCorridor();
    void drawCannon();
    void drawCylindricalGrid();
    void setupLight();
    void drawAxes(); // Nouvelle méthode pour dessiner les axes XYZ

    // Timer pour les animations
    QTimer *timer;

    // Variables pour la projection et la vue
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;

    // Constantes pour les dimensions
    const float corridorLength = 20.0f;
    const float corridorWidth = 5.0f;
    const float corridorHeight = 5.0f;
    const float gridRadius = 3.5f;
    const float gridAngle = 80.0f; // en degrés
};

#endif // MYGLWIDGET_H
