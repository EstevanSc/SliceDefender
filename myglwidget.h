#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <GL/glu.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QMatrix4x4>
#include <QTime>               // Include for QTime
#include "projectileManager.h" // Include for projectile manager
#include "player.h"            // Include for Player class

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
    void drawAxes(); // New method to draw XYZ axes

    /**
     * @brief Position the player's sword on the cylindrical grid using grid coordinates
     *
     * @param gridX X coordinate on the grid (ranges from -1.0 to 1.0)
     *              Where -1.0 is the left edge, 0.0 is center, and 1.0 is the right edge
     * @param gridZ Z coordinate on the grid (ranges from -1.0 to 1.0)
     *              Where -1.0 is the back edge, 0.0 is center, and 1.0 is the front edge
     *
     * This method translates grid coordinates to world coordinates and positions
     * the player's sword directly on the cylindrical grid surface.
     */
    void positionPlayerOnGrid(float gridX = 0.0f, float gridZ = 0.0f);

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

    // For calculating elapsed time between frames
    QTime m_lastFrameTime;

    // Function pointer for game updates
    std::function<void()> m_gameUpdateFunc = nullptr;
};

#endif // MYGLWIDGET_H
