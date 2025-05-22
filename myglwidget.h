#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <GL/glu.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QMatrix4x4>
#include <QTime>
#include "projectileManager.h"
#include <QOpenGLTexture>
#include "player.h"
#include "corridor.h"
#include "keyboardhandler.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget();

    /**
     * @brief Get a pointer to the Player object
     * @return Pointer to the Player instance
     */
    Player *getPlayer() { return &m_player; }

    /**
     * @brief Get a pointer to the ProjectileManager
     * @return Pointer to the ProjectileManager instance
     */
    ProjectileManager *getProjectileManager() { return &m_projectileManager; }

    /**
     * @brief Get a pointer to the KeyboardHandler
     * @return Pointer to the KeyboardHandler instance
     */
    KeyboardHandler *getKeyboardHandler() { return &m_keyboardHandler; }

    /**
     * @brief Set the game update function
     * @param updateFunc Function to call for game logic updates
     *
     * This allows connecting the OpenGL rendering loop with
     * external game logic in the Game class.
     */
    void setGameUpdateFunction(std::function<void()> updateFunc)
    {
        m_gameUpdateFunc = updateFunc;
    }

public slots:
    /**
     * @brief Position the player's sword on the cylindrical grid using grid coordinates
     *
     * @param gridX X coordinate on the grid (ranges from -1.0 to 1.0)
     *              Where -1.0 is the left edge, 0.0 is center, and 1.0 is the right edge
     * @param gridY Y coordinate on the grid (ranges from -1.0 to 1.0)
     *              Where -1.0 is the bottom, 0.0 is center, and 1.0 is the top
     *
     * This method translates grid coordinates to world coordinates and positions
     * the player's sword directly on the cylindrical grid surface. It is connected
     * to the Game class's playerPositionChanged signal to allow continuous movement
     * of the sword based on camera-tracked hand position.
     */
    void positionPlayerOnGrid(float gridX = 0.0f, float gridY = 0.0f);

protected:
    // QOpenGLWidget methods to override
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    // Handle keyboard events
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // Drawing methods
    void drawCorridor();
    void drawCannon();
    void drawCylindricalGrid();
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

    // Player (sword) instance
    Player m_player;

    // Corridor instance (persistent, à initialiser dynamiquement)
    Corridor* m_corridor;

    // Keyboard handler for keyboard controls
    KeyboardHandler m_keyboardHandler;

    // For calculating elapsed time between frames
    QTime m_lastFrameTime;

    // Function pointer for game updates
    std::function<void()> m_gameUpdateFunc = nullptr;
};

#endif // MYGLWIDGET_H
