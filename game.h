#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QTimer>
#include <QVector3D>
#include "player.h"
#include "cameraHandler.h"
#include "projectileManager.h"

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
     * @param projectileManager Pointer to the projectile manager
     * @param parent Parent QObject for memory management
     */
    explicit Game(Player *player, CameraHandler *cameraHandler,
                  ProjectileManager *projectileManager, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~Game();

    /**
     * @brief Update game state - called each frame
     *
     * Updates player position based on camera tracking,
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

    // Game components
    Player *m_player;
    CameraHandler *m_cameraHandler;
    ProjectileManager *m_projectileManager;

    // Game state
    int m_score;
    int m_lives;
    bool m_gameStarted;
    QVector3D m_handPosition;

    // Countdown state
    int m_countdownValue;
    QTimer *m_countdownTimer;
    QTimer *m_updateTimer;
};

#endif // GAME_H