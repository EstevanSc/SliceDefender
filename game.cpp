#include "game.h"
#include <QDebug>

Game::Game(Player *player, CameraHandler *cameraHandler,
           KeyboardHandler *keyboardHandler,
           ProjectileManager *projectileManager, QObject *parent)
    : QObject(parent),
      m_player(player),
      m_cameraHandler(cameraHandler),
      m_keyboardHandler(keyboardHandler),
      m_projectileManager(projectileManager),
      m_score(0),
      m_lives(STANDARD_MODE_LIVES), // Start with Standard Mode lives
      m_gameStarted(false),
      m_countdownValue(5),
      m_pointsCounter(0),
      m_standardMode(true) // Default to Standard Mode
{
    // Initialize timers
    m_updateTimer = new QTimer(this);
    m_countdownTimer = new QTimer(this);

    // Connect countdown timer to slot
    connect(m_countdownTimer, &QTimer::timeout, this, &Game::decrementCountdown);

    // Initialize the hand position
    m_handPosition = QVector3D(0.0f, 0.0f, 0.0f);
    m_playerPosition = QVector3D(0.0f, 0.0f, 0.0f);

    // Set this Game instance in the ProjectileManager
    if (m_projectileManager)
    {
        m_projectileManager->setGame(this);
    }
}

Game::~Game()
{
    // Timers are automatically deleted by Qt parent-child system
}

void Game::update()
{
    // Update player position based on input
    updatePlayerPosition();

    // Return early if game is not running
    if (!m_gameStarted)
    {
        return;
    }
}

void Game::updatePlayerPosition()
{
    // Get current hand position from camera handler
    QVector3D newHandPosition = m_cameraHandler->getHandPosition();

    // Get keyboard movement
    QVector3D keyboardMovement = m_keyboardHandler->getMovementDirection();

    // Only update camera position if there's a significant change and if the camera has a valid detection
    const float MOVEMENT_THRESHOLD = 0.01f;
    bool validPosition = (m_cameraHandler->getTrackedHandPosition() != QPoint(0, 0));
    bool cameraChanged = validPosition && ((newHandPosition - m_handPosition).length() > MOVEMENT_THRESHOLD);

    // Track what changed
    bool positionChanged = false;

    // Update hand position if camera detected significant change
    if (cameraChanged)
    {
        m_handPosition = newHandPosition;
        positionChanged = true;
    }

    // Update with keyboard movement if any keys are pressed
    if (m_keyboardHandler->isMoving())
    {
        // Calculate new position by adding keyboard movement to current player position
        m_playerPosition.setX(qBound(-0.8f, m_playerPosition.x() + keyboardMovement.x(), 0.8f));
        m_playerPosition.setY(qBound(-0.8f, m_playerPosition.y() + keyboardMovement.y(), 0.8f));
        positionChanged = true;
    }
    else if (cameraChanged)
    {
        // Only update player position with camera if no keyboard input
        // Apply smoothing/constraints to prevent extreme movements
        m_playerPosition.setX(qBound(-0.8f, m_handPosition.x(), 0.8f));
        m_playerPosition.setY(qBound(-0.8f, m_handPosition.y(), 0.8f));
    }

    // Only emit position changed signal if position actually changed
    if (positionChanged)
    {

        // Update the player's position on the grid through signal
        emit playerPositionChanged(m_playerPosition.x(), m_playerPosition.y());
    }
}

void Game::resetGame()
{
    // Reset game state based on mode
    m_score = 0;
    m_lives = m_standardMode ? STANDARD_MODE_LIVES : ORIGINAL_MODE_LIVES;
    m_gameStarted = false;
    m_countdownValue = 5;
    m_pointsCounter = 0;

    // Clear any existing projectiles
    m_projectileManager->clearProjectiles();

    // Emit signals to update UI
    emit scoreChanged(m_score);
    emit livesChanged(m_lives);
}

void Game::startCountdown()
{
    // Reset countdown value
    m_countdownValue = 5;

    // Start countdown timer (fires every 1 second)
    m_countdownTimer->start(1000);

    // Emit initial countdown value
    emit countdownUpdated(m_countdownValue);
}

void Game::decrementCountdown()
{
    // Decrease countdown value
    m_countdownValue--;

    // Emit current countdown value
    emit countdownUpdated(m_countdownValue);

    // Check if countdown finished
    if (m_countdownValue <= 0)
    {
        // Stop the countdown timer
        m_countdownTimer->stop();

        // Start the game
        m_gameStarted = true;

        // Activate the projectile manager to start launching projectiles
        m_projectileManager->setGameActive(true);
    }
}

void Game::stopGame()
{
    // Stop the game
    m_gameStarted = false;

    // Disable the projectile manager to stop launching projectiles
    m_projectileManager->setGameActive(false);

    // Emit game ended signal
    emit gameEnded();
}

/**
 * @brief Set the game mode
 * @param standardMode true for standard mode (1 life, 1 point per hit),
 *                    false for original mode (5 lives, combo score system)
 */
void Game::setStandardMode(bool standardMode)
{
    // Only allow mode change when game is not active
    if (!m_gameStarted)
    {
        m_standardMode = standardMode;

        // Update lives based on selected mode
        m_lives = m_standardMode ? STANDARD_MODE_LIVES : ORIGINAL_MODE_LIVES;
        emit livesChanged(m_lives);

        qDebug() << "Game mode set to:" << (m_standardMode ? "Standard" : "Original");
    }
}

/**
 * @brief Increase player's score by one point plus bonus
 * Called when a projectile is sliced by the player
 */
void Game::gainPoint()
{
    if (m_standardMode)
    {
        // Standard mode: always add 1 point
        m_score += 1;
        qDebug() << "Score increased: +1 point | Total score:" << m_score
                 << "| Standard Mode | Lives:" << m_lives;
    }
    else
    {
        // Original mode: combo point system
        m_pointsCounter++;
        m_score += m_pointsCounter;
        qDebug() << "Score increased: +" << m_pointsCounter << " points | Total score:" << m_score
                 << "| Combo:" << m_pointsCounter
                 << "| Lives:" << m_lives;
    }

    emit scoreChanged(m_score);
}

/**
 * @brief Decrease player's lives by one and reset combo counter
 * Called when a projectile hits the ground or passes through the grid
 * @return true if player still has lives remaining, false if game over
 */
bool Game::loseLife()
{
    m_lives--;

    // In original mode, reset combo counter when losing a life
    if (!m_standardMode)
    {
        m_pointsCounter = 0;
    }

    qDebug() << "Life lost: Lives remaining:" << m_lives
             << "| Score:" << m_score
             << (m_standardMode ? "" : "| Combo reset");

    emit livesChanged(m_lives);

    if (m_lives <= 0)
    {
        stopGame();
        return false;
    }

    return true;
}