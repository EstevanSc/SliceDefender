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
 * @brief Main game logic controller that manages game state and scoring
 *
 * This class handles:
 * - Game state (active/inactive)
 * - Score tracking and lives management
 * - Game startup countdown
 * - Player movement through camera tracking and keyboard input
 * 
 * @author Estevan SCHMITT
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
     * Updates player position based on camera tracking and keyboard input
     * and handles game logic.
     */
    void update();

    /**
     * @brief Reset the game to initial state
     *
     * Resets score, lives, and clears all projectiles.
     */
    void resetGame();

    /**
     * @brief Set the game mode
     * @param standardMode true for standard mode (1 life, 1 point per hit),
     *                    false for original mode (5 lives, combo score system)
     */
    void setStandardMode(bool standardMode);

    /**
     * @brief Check if the game is in standard mode
     * @return true if standard mode, false if original mode
     */
    bool isStandardMode() const { return m_standardMode; }

    /**
     * @brief Start the countdown to begin the game
     *
     * Initializes and displays a countdown before the game starts.
     */
    void startCountdown();

    /**
     * @brief Increase player's score by one point plus bonus
     * Called when a projectile is sliced by the player
     * Implements a linear bonus system where each consecutive hit
     * earns progressively more points
     */
    void gainPoint();

    /**
     * @brief Decrease player's lives by one
     * Called when a projectile hits the ground or passes through the grid
     * Also resets the point bonus counter to zero
     * @return true if player still has lives remaining, false if game over
     */
    bool loseLife();

    /**
     * @brief Get the player's sword object
     * @return Pointer to the player
     */
    Player *getPlayer() const { return m_player; }

    /**
     * @brief Check if the game is currently running
     * @return true if game is active, false otherwise
     */
    bool isGameStarted() const { return m_gameStarted; }

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
     * @brief Update player position based on input
     * Combines camera tracking with keyboard controls
     */
    void updatePlayerPosition();

    // Game components
    Player *m_player; // Pointer to the player's sword object
    CameraHandler *m_cameraHandler; // Pointer to the camera handler
    KeyboardHandler *m_keyboardHandler; // Pointer to the keyboard handler
    ProjectileManager *m_projectileManager; // Pointer to the projectile manager

    // Game state
    int m_score; // Current score
    int m_lives; // Current lives
    bool m_gameStarted; // Flag to indicate if the game is active
    QVector3D m_handPosition; // Hand position from camera tracking
    QVector3D m_playerPosition; // Player position on the grid
    int m_pointsCounter; // Counter for consecutive hits
    bool m_standardMode; // true = standard mode, false = original mode

    // Default values for different game modes
    static const int ORIGINAL_MODE_LIVES = 5; // Original mode lives
    static const int STANDARD_MODE_LIVES = 1; // Standard mode lives

    // Countdown state
    int m_countdownValue; // Countdown value
    QTimer *m_countdownTimer; // Timer for countdown
    QTimer *m_updateTimer; // Timer for game updates
};

#endif // GAME_H