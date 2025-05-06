#define _USE_MATH_DEFINES

#include <GL/glu.h>
#include <QApplication>
#include <QTime>
#include "myglwidget.h"
#include <cmath>
#include "cannon.h"

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    // Initialize timer for animations
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(16); // ~60 FPS

    // Initialize last frame time
    m_lastFrameTime = QTime::currentTime();
}

MyGLWidget::~MyGLWidget()
{
    // Clean up resources
    delete timer;
}

void MyGLWidget::initializeGL()
{
    // Initialize OpenGL functions
    initializeOpenGLFunctions();

    // Basic configuration
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // Darker background
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    // Configure matrices
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(QVector3D(0.0f, 2.5f, 0.0f),  // Camera position elevated
                      QVector3D(0.0f, 0.0f, -1.0f), // View direction
                      QVector3D(0.0f, 1.0f, 0.0f)); // Up vector

    // Configure ProjectileManager to launch projectiles from the cannon
    // Position adjusted to match the cannon's mouth
    float cannonLength = 3.0f;
    float cannonAngle = 20.0f * M_PI / 180.0f;

    float cannonPos[3] = {
        0.0f,
        1.2f + cannonLength * std::sin(cannonAngle),
        -corridorLength + cannonLength * std::cos(cannonAngle)};

    // Direction with 20° angle towards the player
    float cannonDir[3] = {0.0f, std::sin(cannonAngle), std::cos(cannonAngle)};

    // Access private attributes via specific ProjectileManager methods
    m_projectileManager.setCannonPosition(cannonPos);
    m_projectileManager.setCannonDirection(cannonDir);
    m_projectileManager.setInitialSpeed(15.0f);
}

void MyGLWidget::resizeGL(int width, int height)
{
    // Adjust viewport
    glViewport(0, 0, width, height);

    // Update projection matrix
    float aspectRatio = static_cast<float>(width) / height;
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, aspectRatio, 0.1f, 100.0f);
}

void MyGLWidget::paintGL()
{
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply projection and view matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspectRatio = static_cast<float>(width()) / height();
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 2.5f, 0.0f,
              0.0f, 0.0f, -corridorLength,
              0.0f, 1.0f, 0.0f);

    // Set up lighting
    setupLight();

    // Draw coordinate axes (X, Y, Z)
    drawAxes();

    // Calculate deltaTime for physics updates
    QTime currentTime = QTime::currentTime();
    float deltaTime = m_lastFrameTime.msecsTo(currentTime) / 1000.0f;
    m_lastFrameTime = currentTime;

    // Limit deltaTime to avoid issues with large values
    if (deltaTime > 0.1f)
        deltaTime = 0.1f;

    // Update projectile manager
    m_projectileManager.update(deltaTime);

    // Draw scene elements
    drawCorridor();
    drawCannon();
    drawCylindricalGrid();

    m_projectileManager.draw();
}

void MyGLWidget::setupLight()
{
    // Configure directional light (downward)
    GLfloat lightPosition[] = {0.0f, 10.0f, 0.0f, 0.0f}; // Directional
    GLfloat lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

void MyGLWidget::drawCorridor()
{
    // Draw corridor
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f); // Gray

    // Floor
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-corridorWidth / 2, 0.0f, 0.0f);
    glVertex3f(corridorWidth / 2, 0.0f, 0.0f);
    glVertex3f(corridorWidth / 2, 0.0f, -corridorLength);
    glVertex3f(-corridorWidth / 2, 0.0f, -corridorLength);
    glEnd();

    // Ceiling
    // glBegin(GL_QUADS);
    // glNormal3f(0.0f, -1.0f, 0.0f);
    // glVertex3f(-corridorWidth / 2, corridorHeight, 0.0f);
    // glVertex3f(-corridorWidth / 2, corridorHeight, -corridorLength);
    // glVertex3f(corridorWidth / 2, corridorHeight, -corridorLength);
    // glVertex3f(corridorWidth / 2, corridorHeight, 0.0f);
    // glEnd();

    // Left wall
    // glBegin(GL_QUADS);
    // glNormal3f(1.0f, 0.0f, 0.0f);
    // glVertex3f(-corridorWidth / 2, 0.0f, 0.0f);
    // glVertex3f(-corridorWidth / 2, 0.0f, -corridorLength);
    // glVertex3f(-corridorWidth / 2, corridotHeight, -corridorLength);
    // glVertex3f(-corridorWidth / 2, corridotHeight, 0.0f);
    // glEnd();

    // Right wall
    // glBegin(GL_QUADS);
    // glNormal3f(-1.0f, 0.0f, 0.0f);
    // glVertex3f(corridorWidth / 2, 0.0f, 0.0f);
    // glVertex3f(corridorWidth / 2, corridotHeight, 0.0f);
    // glVertex3f(corridorWidth / 2, corridotHeight, -corridorLength);
    // glVertex3f(corridorWidth / 2, 0.0f, -corridorLength);
    // glEnd();

    glPopMatrix();
}

void MyGLWidget::drawCannon()
{
    // Draw cannon
    glPushMatrix();

    // Use the Cannon class to draw the cannon
    Cannon cannon;
    cannon.setPosition(QVector3D(0.0f, 0, -corridorLength));
    cannon.setAngle(-20.0f);
    cannon.setLength(3.0f);
    cannon.setRadius(0.8f);
    cannon.setColor(QColor(50, 50, 50));

    // Wheel parameters
    cannon.setWheelRadius(0.7f);
    cannon.setWheelThickness(0.3f);

    cannon.draw();

    glPopMatrix();
}

void MyGLWidget::drawCylindricalGrid()
{
    // Cylinder portion (60°)
    const int segments = 15;
    const float angleStep = gridAngle / segments;
    const float startAngle = -gridAngle / 2;
    const float height = corridorHeight * 0.5f;

    // Draw cylindrical grid (60 degrees)
    glPushMatrix();
    glColor3f(0.0f, 0.8f, 0.8f);    // Cyan
    glTranslatef(0.0f, 2.0f, 0.0f); // Move grid closer to camera

    GLUquadric *quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_LINE); // Wireframe mode

    glBegin(GL_LINES);

    // Longitudinal lines
    for (int i = 0; i <= segments; i++)
    {
        float angle = (startAngle + i * angleStep) * M_PI / 180.0f;
        float x = gridRadius * sin(angle);
        float z = -gridRadius * cos(angle);

        glVertex3f(x, -height / 2, z);
        glVertex3f(x, height / 2, z);
    }

    // Horizontal lines
    const int heightSegments = 10;
    for (int j = 0; j <= heightSegments; j++)
    {
        float y = -height / 2 + j * (height / heightSegments);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= segments; i++)
        {
            float angle = (startAngle + i * angleStep) * M_PI / 180.0f;
            float x = gridRadius * sin(angle);
            float z = -gridRadius * cos(angle);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    glEnd();

    gluDeleteQuadric(quad);
    glPopMatrix();
}

void MyGLWidget::drawAxes()
{
    // Temporarily disable lighting to make axes clearly visible
    glDisable(GL_LIGHTING);

    const float axisLength = 2.0f; // Axes length

    // X axis in red
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(axisLength, 0.0f, 0.0f);
    glEnd();

    // Y axis in green
    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, axisLength, 0.0f);
    glEnd();

    // Z axis in blue
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, axisLength); // Negative Z for depth
    glEnd();

    // Re-enable lighting
    glEnable(GL_LIGHTING);
}