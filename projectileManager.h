#ifndef PROJECTILE_MANAGER_H
#define PROJECTILE_MANAGER_H

#include <vector>
#include <random>
#include "projectile.h"
#include "projectiles/apple.h"
#include "projectiles/orange.h"
#include "projectiles/corn.h"
#include "projectiles/banana.h"

/**
 * @class ProjectileManager
 * @brief Manages all projectiles in Slice Defender (creation, update, cleanup, and slicing).
 *
 * Handles the lifecycle of projectiles, including launching, updating, drawing, collision detection,
 * and slicing logic. Also manages cannon configuration and statistics for projectiles launched.
 *
 * @author: Aubin SIONVILLE, Estevan SCHMITT
 */
class ProjectileManager
{
public:
    /**
     * @brief Constructor. Initializes the projectile manager and random generator.
     */
    ProjectileManager();
    /**
     * @brief Destructor. Cleans up all projectiles.
     */
    ~ProjectileManager();

    /**
     * @brief Updates all projectiles (movement, state, etc.).
     * @param deltaTime Time elapsed since last update (in seconds)
     */
    void update(float deltaTime);
    /**
     * @brief Draws all active projectiles.
     */
    void draw();

    /**
     * @brief Launches a random projectile from the cannon.
     */
    void launchProjectile();

    /**
     * @brief Creates and adds a specific type of projectile.
     * @tparam T Projectile type
     * @tparam Args Constructor arguments
     * @param args Arguments to forward to the projectile constructor
     */
    template <typename T, typename... Args>
    void createProjectile(Args &&...args);

    /**
     * @brief Removes inactive projectiles from the manager.
     */
    void cleanupProjectiles();

    /**
     * @brief Clears all projectiles (for game reset).
     */
    void clearProjectiles();

    /**
     * @brief Sets the position of the cannon.
     * @param position Array of 3 floats [x, y, z]
     */
    void setCannonPosition(const float position[3]);
    /**
     * @brief Sets the direction of the cannon.
     * @param direction Array of 3 floats [dx, dy, dz]
     */
    void setCannonDirection(const float direction[3]);
    /**
     * @brief Sets the initial speed for launched projectiles.
     * @param speed Initial speed value
     */
    void setInitialSpeed(float speed);

    /**
     * @brief Adds a new projectile to the manager.
     * @param projectile Pointer to the projectile to add
     */
    void addProjectile(Projectile *projectile);

    /**
     * @brief Checks all projectiles for slicing and handles split logic.
     */
    void checkProjectilesForSlicing();

    /**
     * @brief Returns a reference to the vector of projectiles (for collision detection, etc.).
     * @return Reference to the vector of projectile pointers
     */
    std::vector<Projectile *> &getProjectiles() { return m_projectiles; }

    /**
     * @brief Sets whether the game is actively running (controls launching).
     * @param active True if the game is active
     */
    void setGameActive(bool active) { m_gameActive = active; }

    /**
     * @brief Sets the Game instance to be passed to projectiles.
     * @param game Pointer to the game instance
     */
    void setGame(Game *game) { m_game = game; }

    /**
     * @brief Returns the number of projectiles launched.
     * @return Number of projectiles launched
     */
    int getProjectilesLaunched() const { return m_projectilesLaunched; }
    /**
     * @brief Resets the projectile launch statistics.
     */
    void resetStats() { m_projectilesLaunched = 0; }

private:
    std::vector<Projectile *> m_projectiles;
    float m_timeSinceLastLaunch;
    static constexpr float LAUNCH_INTERVAL = 2.0f;
    bool m_gameActive = false;
    int m_projectilesLaunched = 0;
    Game *m_game = nullptr; // Reference to the game instance

    // Cannon properties
    float m_cannonPosition[3];
    float m_cannonDirection[3];
    float m_initialProjectileSpeed;

    // Random number generator
    std::mt19937 m_rng;
    std::uniform_int_distribution<int> m_projectileTypeDist{0, 4};
};

#endif // PROJECTILE_MANAGER_H
