#define _USE_MATH_DEFINES

#include <GL/glu.h>
#include <QApplication>
#include <QTime>
#include "myglwidget.h"
#include <cmath>
#include "cannon.h"
#include "corridor.h"
#include "player.h"
#include <QKeyEvent>

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    // Initialize timer for animations
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(16); // ~60 FPS

    // Initialize last frame time
    m_lastFrameTime = QTime::currentTime();

    // Position the player's sword at the center of the cylindrical grid
    // Using (0.0, 0.0) which places it in the center of the grid
    positionPlayerOnGrid(0.0, 0.0);

    // Set focus policy to explicitly capture keyboard input
    setFocusPolicy(Qt::StrongFocus);

    // Accept keyboard input when in focus
    setFocus();

    // Do not initialize m_corridor here (OpenGL not ready)
    m_corridor = nullptr;
}

MyGLWidget::~MyGLWidget()
{
    // Clean up resources
    delete timer;
    if (m_corridor) delete m_corridor;
}

void MyGLWidget::initializeGL()
{
    // Initialize OpenGL functions
    initializeOpenGLFunctions();
    glEnable(GL_NORMALIZE); // Normalize normals after glScalef for correct lighting
    glDisable(GL_COLOR_MATERIAL); // Disable the influence of glColor on materials

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

    // Initialize corridor after OpenGL is ready
    if (m_corridor) delete m_corridor;
    m_corridor = new Corridor();

    // Add a soft global ambient light to avoid blue in the shadows
    float ambientLightStrength = 0.4f;
    GLfloat globalAmbient[4] = {0.18f * ambientLightStrength, 0.16f * ambientLightStrength, 0.13f * ambientLightStrength, 1.0f}; // very soft beige/gray
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
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

    // Call the game update function if it exists
    if (m_gameUpdateFunc)
    {
        m_gameUpdateFunc();
    }

    // Draw scene elements
    if (m_corridor) m_corridor->draw();

    drawCannon();
    drawCylindricalGrid();

    // drawTestObject();

    m_projectileManager.draw();

    // Draw the player's sword at the center of the grid
    // The positioning is handled by the positionPlayerOnGrid method,
    // which ensures the sword is properly aligned with the grid
    m_player.draw();
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


void MyGLWidget::positionPlayerOnGrid(float gridX, float gridY)
{
    // Calculate the angle based on the gridX coordinate and gridAngle
    // gridX ranges from -1.0 (left) to 1.0 (right)
    // Convert to an angle within the gridAngle range
    float angle = (gridX * (gridAngle / 2.0f)) * M_PI / 180.0f;

    // Get the base Y-coordinate of the grid (height)
    // For a cylindrical grid centered at y=2.0 (as in drawCylindricalGrid)
    float baseY = 2.0f;

    // Calculate the height offset based on gridY
    // gridY ranges from -1.0 (bottom) to 1.0 (top)
    // Scale to move within the grid height limits
    float heightOffset = gridY * (corridorHeight * 0.25f); // Half of half height
    float worldY = baseY + heightOffset;

    // Use a fixed radius for the grid cylinder
    float radius = gridRadius;

    // Convert from polar coordinates (angle, radius) to Cartesian coordinates (x, z)
    float worldX = radius * std::sin(angle);
    float worldZ = -radius * std::cos(angle); // Negative for OpenGL z-axis orientation

    // Set the player's position directly on the grid surface
    m_player.setPosition(QVector3D(worldX, worldY, worldZ));

    // Set the player's rotation to face perpendicular to the grid surface
    // The Y rotation follows the curvature of the grid
    float rotationY = angle * 180.0f / M_PI;
    m_player.setRotation(0.0f, rotationY, 0.0f);
}

void MyGLWidget::keyPressEvent(QKeyEvent *event)
{
    // Handle numpad keys specifically, which sometimes need special treatment
    int key = event->key();

    // Map numpad keys to their corresponding standard keys if needed
    if (event->modifiers() & Qt::KeypadModifier)
    {
        switch (key)
        {
        case Qt::Key_Plus:
            key = Qt::Key_Plus; // Keep as is
            break;
        case Qt::Key_Minus:
            key = Qt::Key_Minus; // Keep as is
            break;
        }
    }

    // Pass key events to the keyboard handler with possibly modified key
    m_keyboardHandler.keyPressed(key);

    // Allow standard processing
    QOpenGLWidget::keyPressEvent(event);
}

void MyGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    // Pass key events to the keyboard handler
    m_keyboardHandler.keyReleased(event->key());

    // Allow standard processing
    QOpenGLWidget::keyReleaseEvent(event);
}