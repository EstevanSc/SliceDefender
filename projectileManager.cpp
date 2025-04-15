#include "projectileManager.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <QDebug>

ProjectileManager::ProjectileManager()
    : m_timeSinceLastLaunch(0.0f), m_initialProjectileSpeed(15.0f),
      m_rng(std::random_device()()), m_projectileTypeDist(0, 1)
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

    // Launch new projectile if it's time
    if (m_timeSinceLastLaunch >= LAUNCH_INTERVAL)
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
    // Distribution generator to slightly randomize velocity in X and Y
    std::uniform_real_distribution<float> randomVerticalOffset(-1.5f, 0.5f);
    std::uniform_real_distribution<float> randomHorizontalOffset(-1.5f, 1.5f);

    // Calculate initial velocity based on cannon direction, initial speed and random offsets
    float velocityX = (m_cannonDirection[0] * m_initialProjectileSpeed) + randomHorizontalOffset(m_rng);
    float velocityY = (m_cannonDirection[1] * m_initialProjectileSpeed) + randomVerticalOffset(m_rng);
    float velocityZ = m_cannonDirection[2] * m_initialProjectileSpeed;

    // Debug - display initial velocity
    qDebug() << "Launch velocity: (" << velocityX << ", " << velocityY << ", " << velocityZ << ")";
    qDebug() << "Launch position: (" << m_cannonPosition[0] << ", " << m_cannonPosition[1] << ", " << m_cannonPosition[2] << ")";

    // Randomly choose projectile type to launch
    int projectileType = m_projectileTypeDist(m_rng);

    Projectile *newProjectile = nullptr;

    // Adjust projectile size for better visibility
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
    // Remove inactive projectiles
    auto it = std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                             [](Projectile *p)
                             {
                                 if (!p->isActive())
                                 {
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
        // Check if the projectile should be sliced (property shouldSlice)
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
