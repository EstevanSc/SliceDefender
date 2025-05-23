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

/**
 * @class MyGLWidget
 * @brief Main OpenGL widget for 3D rendering of the Slice Defender game.
 *
 * Handles OpenGL rendering, keyboard input management, player (sword) position,
 * projectile management, game corridor, and synchronization with game logic.
 * Acts as a bridge between the game logic (Game) and the graphical display.
 *
 * @author Estevan SCHMITT, Aubin SIONVILLE
 */
class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    /**
     * @brief Constructor. Initializes the OpenGL widget and associated resources.
     * @param parent Parent widget
     */
    explicit MyGLWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructor. Releases OpenGL resources and dynamic objects.
     */
    ~MyGLWidget();

    /**
     * @brief Returns a pointer to the Player object (player's sword)
     * @return Pointer to the Player instance
     */
    Player *getPlayer() { return &m_player; }

    /**
     * @brief Returns a pointer to the projectile manager
     * @return Pointer to the ProjectileManager instance
     */
    ProjectileManager *getProjectileManager() { return &m_projectileManager; }

    /**
     * @brief Returns a pointer to the keyboard handler
     * @return Pointer to the KeyboardHandler instance
     */
    KeyboardHandler *getKeyboardHandler() { return &m_keyboardHandler; }

    /**
     * @brief Sets the game logic update function
     * @param updateFunc Function to call for game updates
     *
     * Allows connecting the OpenGL render loop to external game logic (Game class).
     */
    void setGameUpdateFunction(std::function<void()> updateFunc)
    {
        m_gameUpdateFunc = updateFunc;
    }

    /**
     * @brief Positions the player's sword on the cylindrical grid from grid coordinates
     *
     * @param gridX X coordinate on the grid (from -1.0 to 1.0)
     * @param gridY Y coordinate on the grid (from -1.0 to 1.0)
     *
     * Converts grid coordinates to world coordinates and positions the sword on the surface of the cylindrical grid.
     * Connected to the playerPositionChanged signal of the Game class for continuous hand tracking.
     */
    void positionPlayerOnGrid(float gridX = 0.0f, float gridY = 0.0f);

protected:
    // QOpenGLWidget methods to override
    /**
     * @brief Initializes the OpenGL context, shaders, and scene objects.
     */
    void initializeGL() override;
    /**
     * @brief Renders the 3D scene every frame.
     */
    void paintGL() override;
    /**
     * @brief Handles resizing of the OpenGL window.
     * @param width New width
     * @param height New height
     */
    void resizeGL(int width, int height) override;

    /**
     * @brief Handles key press events.
     * @param event Keyboard event
     */
    void keyPressEvent(QKeyEvent *event) override;
    /**
     * @brief Handles key release events.
     * @param event Keyboard event
     */
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // Internal drawing methods
    /**
     * @brief Draws the projectile launching cannon.
     */
    void drawCannon();
    /**
     * @brief Draws the cylindrical grid on which the sword moves.
     */
    void drawCylindricalGrid();
    /**
     * @brief Draws reference axes (XYZ).
     */
    void drawAxes();
    /**
     * @brief Draws a test object (for debugging).
     */
    void drawTestObject();

    QTimer *timer; // Timer for periodic updates

    QMatrix4x4 projectionMatrix; // Projection matrix for 3D rendering
    QMatrix4x4 viewMatrix; // View matrix for camera position

    const float corridorLength = 20.0f; // Length of the corridor
    const float corridorWidth = 5.0f; // Width of the corridor
    const float corridorHeight = 5.0f; // Height of the corridor
    const float gridRadius = 3.5f; // Radius of the cylindrical grid
    const float gridAngle = 60.0f; // Angle of the cylindrical grid

    ProjectileManager m_projectileManager; // Manages all projectiles in the game
    Player m_player; // Represents the player's sword
    Corridor* m_corridor; // Pointer to the corridor object
    KeyboardHandler m_keyboardHandler; // Handles keyboard input
    QTime m_lastFrameTime; // Tracks the last frame time
    std::function<void()> m_gameUpdateFunc = nullptr; // Game update function
};

#endif // MYGLWIDGET_H

