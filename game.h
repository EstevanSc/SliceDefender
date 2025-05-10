#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QTimer>
#include <QVector3D>
#include "player.h"
#include "cameraHandler.h"
#include "projectileManager.h"
#include "keyboardhandler.h"

/**
 * @class Game
 * @brief Main game logic controller that manages game state, collision detection and scoring
 *
 * This class handles:
 * - Game state (active/inactive)
 * - Collision detection between projectiles and the player's sword
 * - Score tracking and lives management
 * - Game startup countdown
 */
class Game : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor initializes game with required components
     * @param player Pointer to the player (sword) object
     * @param cameraHandler Pointer to the camera handler for tracking hand position
     * @param keyboardHandler Pointer to the keyboard handler for keyboard input
     * @param projectileManager Pointer to the projectile manager
     * @param parent Parent QObject for memory management
     */
    explicit Game(Player *player, CameraHandler *cameraHandler,
                  KeyboardHandler *keyboardHandler,
                  ProjectileManager *projectileManager, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~Game();

    /**
     * @brief Update game state - called each frame
     *
     * Updates player position based on camera tracking and keyboard input,
     * checks for collisions, and handles game logic.
     */
    void update();

    /**
     * @brief Reset the game to initial state
     *
     * Resets score, lives, and clears all projectiles.
     */
    void resetGame();

    /**
     * @brief Start the countdown to begin the game
     *
     * Initializes and displays a countdown before the game starts.
     */
    void startCountdown();

    /**
     * @brief Check if the game is currently active
     * @return True if the game is running, false otherwise
     */
    bool isGameActive() const { return m_gameStarted; }

signals:
    /**
     * @brief Signal emitted when the score changes
     * @param score The new score value
     */
    void scoreChanged(int score);

    /**
     * @brief Signal emitted when number of lives changes
     * @param lives The new lives value
     */
    void livesChanged(int lives);

    /**
     * @brief Signal emitted when countdown value changes
     * @param value The new countdown value
     */
    void countdownUpdated(int value);

    /**
     * @brief Signal emitted when the game ends
     */
    void gameEnded();

    /**
     * @brief Signal emitted when the player's position should change
     * @param gridX X-coordinate on the grid (-1 to 1)
     * @param gridZ Z-coordinate on the grid (-1 to 1)
     *
     * This signal connects the Game class to the MyGLWidget to update
     * the player's position on the cylindrical grid based on hand tracking.
     */
    void playerPositionChanged(float gridX, float gridZ);

private slots:
    /**
     * @brief Decrements the countdown timer and starts game when countdown reaches zero
     */
    void decrementCountdown();

private:
    /**
     * @brief Stops the active game
     */
    void stopGame();

    /**
     * @brief Checks for collisions between projectiles and player's sword
     *
     * Determines if projectiles:
     * - Collide with the player's sword (giving points)
     * - Pass through the grid zone without being sliced (losing lives)
     * - Hit the ground (losing lives)
     */
    void checkCollisions();

    /**
     * @brief Update player position based on input
     * Combines camera tracking with keyboard controls
     */
    void updatePlayerPosition();

    // Game components
    Player *m_player;
    CameraHandler *m_cameraHandler;
    KeyboardHandler *m_keyboardHandler;
    ProjectileManager *m_projectileManager;

    // Game state
    int m_score;
    int m_lives;
    bool m_gameStarted;
    QVector3D m_handPosition;
    QVector3D m_playerPosition;

    // Countdown state
    int m_countdownValue;
    QTimer *m_countdownTimer;
    QTimer *m_updateTimer;
};

#endif // GAME_H