#include "projectileManager.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <QDebug>

ProjectileManager::ProjectileManager()
    : m_timeSinceLastLaunch(0.0f), m_initialProjectileSpeed(15.0f),
      m_rng(std::random_device()()), m_projectileTypeDist(0, 1), m_projectilesLaunched(0)
{
    // Default values replaced by setter methods
    m_cannonPosition[0] = 0.0f;
    m_cannonPosition[1] = 0.0f;
    m_cannonPosition[2] = -20.0f;

    m_cannonDirection[0] = 0.0f;
    m_cannonDirection[1] = 0.0f;
    m_cannonDirection[2] = -1.0f;

    // Normalization done in the setCannonDirection method
}

ProjectileManager::~ProjectileManager()
{
    // Clean up all projectiles
    for (auto projectile : m_projectiles)
    {
        delete projectile;
    }
    m_projectiles.clear();
}

void ProjectileManager::update(float deltaTime)
{
    // Update timer for launching projectiles
    m_timeSinceLastLaunch += deltaTime;

    // Only launch new projectiles if the game is active
    if (m_gameActive && m_timeSinceLastLaunch >= LAUNCH_INTERVAL)
    {
        // Debug print
        qDebug() << "Launching projectile from cannon at position: ("
                 << m_cannonPosition[0] << ", " << m_cannonPosition[1] << ", "
                 << m_cannonPosition[2] << ")";
        launchProjectile();
        m_timeSinceLastLaunch = 0.0f;
    }

    // Check for projectiles that should be sliced
    checkProjectilesForSlicing();

    // Update all projectiles
    for (auto projectile : m_projectiles)
    {
        projectile->update(deltaTime);
    }

    // Clean up inactive projectiles
    cleanupProjectiles();
}

void ProjectileManager::draw()
{
    for (auto projectile : m_projectiles)
    {
        projectile->draw();
    }
}

void ProjectileManager::launchProjectile()
{
    // Random offset generators for projectile trajectory variation
    std::uniform_real_distribution<float> randomVerticalOffset(-1.5f, 0.5f);
    std::uniform_real_distribution<float> randomHorizontalOffset(-1.5f, 1.5f);

    // Calculate velocity with directional vector and random offsets for gameplay variety
    float velocityX = (m_cannonDirection[0] * m_initialProjectileSpeed) + randomHorizontalOffset(m_rng);
    float velocityY = (m_cannonDirection[1] * m_initialProjectileSpeed) + randomVerticalOffset(m_rng);
    float velocityZ = m_cannonDirection[2] * m_initialProjectileSpeed;

    // Increment launched projectile counter
    m_projectilesLaunched++;

    // Log diagnostic information
    qDebug() << "Launch velocity: (" << velocityX << ", " << velocityY << ", " << velocityZ << ")";
    qDebug() << "Launch position: (" << m_cannonPosition[0] << ", " << m_cannonPosition[1] << ", " << m_cannonPosition[2] << ")";
    qDebug() << "Total projectiles launched: " << m_projectilesLaunched;

    // Select random projectile type (apple or orange)
    int projectileType = m_projectileTypeDist(m_rng);
    Projectile *newProjectile = nullptr;

    // Create the selected projectile type
    if (projectileType == 0)
    {
        newProjectile = new Apple(
            m_cannonPosition[0], m_cannonPosition[1], m_cannonPosition[2],
            velocityX, velocityY, velocityZ);
    }
    else
    {
        newProjectile = new Orange(
            m_cannonPosition[0], m_cannonPosition[1], m_cannonPosition[2],
            velocityX, velocityY, velocityZ);
    }

    // Add projectile to active projectiles list
    if (newProjectile)
    {
        m_projectiles.push_back(newProjectile);
    }
}

template <typename T, typename... Args>
void ProjectileManager::createProjectile(Args &&...args)
{
    static_assert(std::is_base_of<Projectile, T>::value, "T must inherit from Projectile");
    m_projectiles.push_back(new T(std::forward<Args>(args)...));
}

void ProjectileManager::cleanupProjectiles()
{
    // Only remove inactive projectiles after they've been properly processed
    // This avoids destroying projectiles immediately upon contact with grid/player
    auto it = std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                             [](Projectile *p)
                             {
                                 if (!p->isActive())
                                 {
                                     const std::type_info &typeInfo = typeid(*p);
                                     const char *typeName = typeInfo.name();

                                     float *pos = p->getPosition();
                                     qDebug() << "Cleaning up inactive projectile:" << typeName
                                              << "at position:" << pos[0] << "," << pos[1] << "," << pos[2];

                                     delete p;
                                     return true;
                                 }
                                 return false;
                             });

    m_projectiles.erase(it, m_projectiles.end());
}

void ProjectileManager::setCannonPosition(const float position[3])
{
    m_cannonPosition[0] = position[0];
    m_cannonPosition[1] = position[1];
    m_cannonPosition[2] = position[2];
}

void ProjectileManager::setCannonDirection(const float direction[3])
{
    // Copy direction
    m_cannonDirection[0] = direction[0];
    m_cannonDirection[1] = direction[1];
    m_cannonDirection[2] = direction[2];

    // Normalize direction vector
    float length = std::sqrt(m_cannonDirection[0] * m_cannonDirection[0] +
                             m_cannonDirection[1] * m_cannonDirection[1] +
                             m_cannonDirection[2] * m_cannonDirection[2]);

    if (length > 0.0001f)
    { // Avoid division by zero
        m_cannonDirection[0] /= length;
        m_cannonDirection[1] /= length;
        m_cannonDirection[2] /= length;
    }
}

void ProjectileManager::setInitialSpeed(float speed)
{
    m_initialProjectileSpeed = speed;
}

void ProjectileManager::checkProjectilesForSlicing()
{
    for (auto projectile : m_projectiles)
    {
        // Check if the projectile should be sliced (based on the shouldSlice flag)
        // This flag is now only activated by collision with the sword
        if (projectile->shouldSlice())
        {
            qDebug() << "Slicing projectile at position: ("
                     << projectile->getPosition()[0] << ", "
                     << projectile->getPosition()[1] << ", "
                     << projectile->getPosition()[2] << ")";

            // Let the projectile create its own sliced pieces
            projectile->slice(this);

            // Mark the projectile as sliced
            projectile->setSliced();
        }
    }
}

void ProjectileManager::addProjectile(Projectile *projectile)
{
    if (projectile)
    {
        m_projectiles.push_back(projectile);
    }
}

void ProjectileManager::clearProjectiles()
{
    // Delete all projectiles
    for (auto projectile : m_projectiles)
    {
        delete projectile;
    }
    m_projectiles.clear();

    // Reset timer
    m_timeSinceLastLaunch = 0.0f;
}
