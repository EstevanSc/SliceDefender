#ifndef PROJECTILE_MANAGER_H
#define PROJECTILE_MANAGER_H

#include <vector>
#include <memory>
#include <random>
#include "projectile.h"
#include "projectiles/apple.h"
#include "projectiles/orange.h"

class ProjectileManager
{
public:
    ProjectileManager();
    ~ProjectileManager();

    void update(float deltaTime);
    void draw();

    // Launch a random projectile from the cannon
    void launchProjectile();

    // Create a specific type of projectile
    template <typename T, typename... Args>
    void createProjectile(Args &&...args);

    // Clean up inactive projectiles
    void cleanupProjectiles();

    // Methods to configure the cannon
    void setCannonPosition(const float position[3]);
    void setCannonDirection(const float direction[3]);
    void setInitialSpeed(float speed);

    // Add a new projectile to the manager
    void addProjectile(Projectile *projectile);

    // Method to handle projectile slicing
    void checkProjectilesForSlicing();

private:
    std::vector<Projectile *> m_projectiles;
    float m_timeSinceLastLaunch;
    static constexpr float LAUNCH_INTERVAL = 2.0f;

    // Cannon properties
    float m_cannonPosition[3];
    float m_cannonDirection[3];
    float m_initialProjectileSpeed;

    // Random number generator
    std::mt19937 m_rng;
    std::uniform_int_distribution<int> m_projectileTypeDist;
};

#endif // PROJECTILE_MANAGER_H
