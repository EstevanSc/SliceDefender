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
}

Game::~Game()
{
    // Timers are automatically deleted by Qt parent-child system
}

void Game::update()
{
    // Return early if game is not running
    if (!m_gameStarted)
    {
        return;
    }

    // Get current hand position from camera handler
    m_handPosition = m_cameraHandler->getHandPosition();

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
    const float COLLISION_THRESHOLD = 1.2f; // Detection radius around the sword

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
        const std::type_info &typeInfo = typeid(*projectile);
        const char *typeName = typeInfo.name();
        bool isHalf = (strstr(typeName, "Half") != nullptr);

        // Check for floor collision
        if (projectilePos.y() <= 0.0f)
        {
            qDebug() << "Floor collision: Projectile" << typeName << "at" << projectilePos;

            // Only whole projectiles cause life loss
            if (!isHalf)
            {
                m_lives--;
                qDebug() << "Life lost (floor): Lives remaining:" << m_lives
                         << "| Score:" << m_score
                         << "| Total projectiles:" << m_projectileManager->getProjectilesLaunched();

                emit livesChanged(m_lives);

                if (m_lives <= 0)
                {
                    stopGame();
                }
            }

            projectile->setActive(false);
            continue;
        }

        // Check if projectile is in grid zone
        bool isInGridZone = (projectilePos.z() >= GRID_Z_POSITION - GRID_THICKNESS &&
                             projectilePos.z() <= GRID_Z_POSITION + GRID_THICKNESS &&
                             std::abs(projectilePos.y() - GRID_Y_POSITION) <= GRID_THICKNESS);

        // Check collision with player's sword
        float distance = (projectilePos - playerSwordPos).length();
        bool collidesWithSword = (distance < COLLISION_THRESHOLD + projectile->getRadius());

        // Handle sword collision - slice projectile
        if (collidesWithSword)
        {
            qDebug() << "Sword collision: Projectile" << typeName << "sliced at" << projectilePos
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
            }

            projectile->setActive(false);
            continue;
        }

        // Handle projectile passing through grid without being sliced
        if (isInGridZone && !isHalf && projectilePos.z() > GRID_Z_POSITION)
        {
            m_lives--;
            qDebug() << "Grid missed: Life lost! Projectile" << typeName << "passed through grid at" << projectilePos
                     << "Lives remaining:" << m_lives
                     << "| Score:" << m_score;

            emit livesChanged(m_lives);

            if (m_lives <= 0)
            {
                stopGame();
            }

            projectile->setActive(false);
            continue;
        }
    }
}