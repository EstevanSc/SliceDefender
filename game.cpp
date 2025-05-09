#include "game.h"
#include <QDebug>

Game::Game(Player *player, CameraHandler *cameraHandler,
           ProjectileManager *projectileManager, QObject *parent)
    : QObject(parent),
      m_player(player),
      m_cameraHandler(cameraHandler),
      m_projectileManager(projectileManager),
      m_score(0),
      m_lives(5),
      m_gameStarted(false),
      m_countdownValue(5)
{
    // Initialize timers
    m_updateTimer = new QTimer(this);
    m_countdownTimer = new QTimer(this);

    // Connect countdown timer to slot
    connect(m_countdownTimer, &QTimer::timeout, this, &Game::decrementCountdown);

    // Initialize the hand position
    m_handPosition = QVector3D(0.0f, 0.0f, 0.0f);
}

Game::~Game()
{
    // Timers are automatically deleted by Qt parent-child system
}

void Game::update()
{
    // Get current hand position from camera handler
    // This is done regardless of game state to allow sword movement at all times
    QVector3D newHandPosition = m_cameraHandler->getHandPosition();

    // Only update if there's a significant change and if the camera has a valid detection
    const float MOVEMENT_THRESHOLD = 0.01f;
    bool validPosition = (m_cameraHandler->getTrackedHandPosition() != QPoint(0, 0));

    if (validPosition && ((newHandPosition - m_handPosition).length() > MOVEMENT_THRESHOLD))
    {
        m_handPosition = newHandPosition;

        // Map the hand position to player position on the cylindrical grid
        // X coordinate (left-right) maps directly to the grid X (-1 to 1)
        // Y coordinate (up-down) now maps to grid Y instead of Z
        // This fixes the issue where up/down movements were causing forward/backward motion

        // Apply smoothing/constraints to prevent extreme movements
        float gridX = qBound(-0.8f, m_handPosition.x(), 0.8f);

        // Change this: now Y coordinate controls the vertical position (up/down)
        // and Z coordinate (unused in camera tracking) is set to a fixed value
        float gridY = qBound(-0.8f, m_handPosition.y(), 0.8f);
        float gridZ = 0.0f; // Fixed Z position (neither forward nor backward)

        // Log the sword position for debugging
        qDebug() << "Moving sword to grid position: (" << gridX << ", " << gridY << ", " << gridZ << ")";

        // Update the player's position on the grid through signal
        // Now using gridY for up/down movement instead of gridZ
        emit playerPositionChanged(gridX, gridY);
    }

    // Return early if game is not running
    if (!m_gameStarted)
    {
        return;
    }

    // Check for collisions between projectiles and player
    checkCollisions();
}

void Game::resetGame()
{
    // Reset game state
    m_score = 0;
    m_lives = 5;
    m_gameStarted = false;
    m_countdownValue = 5;

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

void Game::checkCollisions()
{
    // Collision detection parameters
    // Make the sword hit box much smaller to limit it to just the sword
    const float COLLISION_THRESHOLD = 0.3f; // Reduced from 1.2f to make hitbox tighter

    // Grid zone definition - where projectiles should be sliced
    const float GRID_Z_POSITION = -3.5f; // Z position of cylindrical grid
    const float GRID_Y_POSITION = 2.0f;  // Y position of grid (height)
    const float GRID_THICKNESS = 3.0f;   // Collision detection zone thickness

    // Log game state for debugging
    qDebug() << "Game state: Sword position:" << m_player->getPosition()
             << "Active projectiles:" << m_projectileManager->getProjectiles().size()
             << "Score:" << m_score
             << "Lives:" << m_lives;

    // Access projectiles vector and sword position
    auto &projectiles = m_projectileManager->getProjectiles();
    QVector3D playerSwordPos = m_player->getPosition();

    // Get player blade position more precisely
    // The blade is positioned above the handle and guard
    float handleLength = 0.5f / 3.0f; 
    float guardHeight = 0.05f / 3.0f; 
    float bladeLength = 1.0f / 3.0f;  

    // Calculate more accurate sword blade position (center of the blade)
    QVector3D swordBladePos = playerSwordPos;
    swordBladePos.setY(playerSwordPos.y() + (handleLength + guardHeight + bladeLength / 2) *
                                                cos(m_player->getRotation().x() * M_PI / 180.0f));

    // Process each active projectile
    for (size_t i = 0; i < projectiles.size(); ++i)
    {
        Projectile *projectile = projectiles[i];

        // Skip inactive projectiles
        if (!projectile->isActive())
        {
            continue;
        }

        // Get projectile position and type information
        float *pos = projectile->getPosition();
        QVector3D projectilePos(pos[0], pos[1], pos[2]);
        bool isHalf = projectile->isHalf();

        // Check for floor collision - ONLY MARK INACTIVE, DON'T DESTROY
        if (projectilePos.y() <= 0.0f)
        {
            qDebug() << "Floor collision: Projectile at" << projectilePos;

            // Only whole projectiles cause life loss, and only if they haven't already decreased a life
            if (!isHalf && !projectile->hasDecreasedLife())
            {
                m_lives--;
                projectile->setDecreasedLife(true);

                qDebug() << "Life lost (floor): Lives remaining:" << m_lives
                         << "| Score:" << m_score
                         << "| Total projectiles:" << m_projectileManager->getProjectilesLaunched();

                emit livesChanged(m_lives);

                if (m_lives <= 0)
                {
                    stopGame();
                }
            }

            // Mark as inactive but don't destroy yet
            projectile->setActive(false);
            continue;
        }

        // Check if projectile is in grid zone
        bool isInGridZone = (projectilePos.z() >= GRID_Z_POSITION - GRID_THICKNESS &&
                             projectilePos.z() <= GRID_Z_POSITION + GRID_THICKNESS &&
                             std::abs(projectilePos.y() - GRID_Y_POSITION) <= GRID_THICKNESS);

        // Check collision with player's sword - use more precise blade position and smaller threshold
        float distance = (projectilePos - swordBladePos).length();
        bool collidesWithSword = (distance < COLLISION_THRESHOLD + projectile->getRadius());

        // Handle sword collision - slice projectile
        if (collidesWithSword)
        {
            qDebug() << "Sword collision: Projectile sliced at" << projectilePos
                     << "Distance:" << distance;

            // Only process slicing for whole projectiles
            if (!isHalf)
            {
                projectile->setShouldSlice(true);
                projectile->slice(m_projectileManager);

                m_score++;

                qDebug() << "Score increased: New score:" << m_score
                         << "| Lives:" << m_lives
                         << "| Total projectiles:" << m_projectileManager->getProjectilesLaunched();

                emit scoreChanged(m_score);

                // Mark as inactive but let the slice method handle creating the halves
                projectile->setActive(false);
            }

            continue;
        }

        // Handle projectile passing through grid without being sliced
        if (isInGridZone && !isHalf && projectilePos.z() > GRID_Z_POSITION && !projectile->hasDecreasedLife())
        {
            m_lives--;
            projectile->setDecreasedLife(true);

            qDebug() << "Grid missed: Life lost! Projectile passed through grid at" << projectilePos
                     << "Lives remaining:" << m_lives
                     << "| Score:" << m_score;

            emit livesChanged(m_lives);

            if (m_lives <= 0)
            {
                stopGame();
            }

            continue;
        }
    }
}