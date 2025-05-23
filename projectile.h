#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <vector>
#include <QVector3D>
#include "player.h"

// NB : These are needed forward declaration to prevent issues
// due to circular includes
class ProjectileManager;
class Game;

/**
 * @class Projectile
 * @brief Abstract base class for all projectiles in Slice Defender.
 *
 * Represents a moving object in the game world, with physics (position, velocity, acceleration),
 * collision detection, slicing logic, and rendering (including shadow).
 * Provides an interface for specialized projectiles (e.g., fruits, halves)
 * and manages state such as activity, slicing, and life loss.
 *
 * @author: Aubin SIONVILLE, Estevan SCHMITT 
 */
class Projectile
{
public:
    /**
     * @brief Constructor. Initializes projectile position and velocity.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     */
    Projectile(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    virtual ~Projectile() = default;

    /**
     * @brief Draws the projectile (pure virtual, implemented by subclasses).
     */
    virtual void draw() = 0;
    /**
     * @brief Updates the projectile's state (position, velocity, etc.) for the given time step.
     * @param deltaTime Time elapsed since last update (in seconds)
     */
    virtual void update(float deltaTime);

    // Slice-related methods
    /**
     * @brief Slices the projectile (pure virtual, implemented by subclasses).
     * @param manager Pointer to the projectile manager
     */
    virtual void slice(ProjectileManager *manager) = 0;
    /**
     * @brief Returns true if the projectile should be sliced.
     */
    bool shouldSlice() const;
    /**
     * @brief Marks the projectile as sliced.
     */
    void setSliced();
    /**
     * @brief Sets the shouldSlice flag.
     * @param value True if the projectile should be sliced
     */
    void setShouldSlice(bool value);
    /**
     * @brief Returns the projectile's collision radius (pure virtual).
     */
    virtual float getRadius() const = 0;

    // Collision detection methods
    /**
     * @brief Checks if the projectile is near the player.
     * @param player Reference to the player
     * @return True if near the player
     */
    bool isNearPlayer(const Player &player) const;
    /**
     * @brief Checks if the projectile is in the grid zone.
     * @param gridZPosition Z position of the grid
     * @param gridYPosition Y position of the grid
     * @param gridThickness Thickness of the grid
     * @return True if in the grid zone
     */
    bool isInGridZone(float gridZPosition, float gridYPosition, float gridThickness) const;
    /**
     * @brief Checks if the projectile is in the grid zone and has passed through it.
     * @param gridZPosition Z position of the grid
     * @param gridYPosition Y position of the grid
     * @param gridThickness Thickness of the grid
     * @return True if in the grid zone and passed
     */
    bool isInGridZoneAndPassed(float gridZPosition, float gridYPosition, float gridThickness) const;
    /**
     * @brief Returns true if the projectile has touched the floor.
     */
    bool hasTouchedFloor() const;
    /**
     * @brief Checks and handles collision with the player.
     * @param player Pointer to the player
     */
    void checkCollisionWithPlayer(Player *player);

    // Getters
    /**
     * @brief Returns a pointer to the projectile's position array [x, y, z].
     */
    float *getPosition() const;
    /**
     * @brief Returns a pointer to the projectile's velocity array [vx, vy, vz].
     */
    float *getVelocity() const;
    /**
     * @brief Returns a pointer to the projectile's acceleration array [ax, ay, az].
     */
    float *getAcceleration() const;

    // Setters
    /**
     * @brief Sets the projectile's position.
     * @param x X position
     * @param y Y position
     * @param z Z position
     */
    void setPosition(float x, float y, float z);
    /**
     * @brief Sets the projectile's velocity.
     * @param vx X velocity
     * @param vy Y velocity
     * @param vz Z velocity
     */
    void setVelocity(float vx, float vy, float vz);
    /**
     * @brief Sets the projectile's acceleration.
     * @param ax X acceleration
     * @param ay Y acceleration
     * @param az Z acceleration
     */
    void setAcceleration(float ax, float ay, float az);
    /**
     * @brief Sets the projectile's rotation axis.
     * @param x X component
     * @param y Y component
     * @param z Z component
     */
    void setRotationAxis(float x, float y, float z);
    /**
     * @brief Sets the projectile's rotation speed.
     * @param speed Rotation speed (degrees/sec)
     */
    void setRotationSpeed(float speed);
    /**
     * @brief Sets the projectile's rotation time.
     * @param t Time value
     */
    void setRotationTime(float t);
    /**
     * @brief Sets the pointer to the Game instance.
     * @param game Pointer to the game
     */
    void setGame(Game *game) { m_game = game; }

    /**
     * @brief Returns true if the projectile is active (in play).
     */
    bool isActive() const;
    /**
     * @brief Sets the projectile's active state.
     * @param active True if active
     */
    void setActive(bool active);

    /**
     * @brief Returns true if the projectile is a half (for split fruits).
     */
    virtual bool isHalf() const { return false; }

    // Life loss tracking
    /**
     * @brief Returns true if the projectile has already caused a life loss.
     */
    bool hasDecreasedLife() const { return m_hasDecreasedLife; }
    /**
     * @brief Sets the life loss flag for this projectile.
     * @param decreased True if life has been decreased
     */
    void setDecreasedLife(bool decreased) { m_hasDecreasedLife = decreased; }

    // Shadow rendering method
    /**
     * @brief Draws the projectile's shadow.
     */
    void drawShadow() const;
    
protected:
    /**
     * @brief Calculates the scale of the projectile's shadow based on its position.
     * @return The calculated shadow scale.
     */
    float calculateShadowScale() const;

    float m_position[3]; // Position in world coordinates (x, y, z)
    float m_velocity[3]; // Velocity in world coordinates (vx, vy, vz)
    float m_acceleration[3]; // Acceleration in world coordinates (ax, ay, az)
    bool m_isActive; // Flag to indicate if the projectile is active
    bool m_sliced = false; // Flag to indicate if the projectile has been sliced
    bool m_shouldSlice = false; // Flag to indicate if the projectile should be sliced
    bool m_hasDecreasedLife = false; // Flag to track if projectile has already caused life loss

    // For rotation
    float m_rotationAxis[3] = {0.0f, 1.0f, 0.0f}; // Rotation axis (x, y, z)
    float m_rotationAngle = 0.0f; // Rotation angle (degrees)
    float m_rotationSpeed = 0.0f; // Rotation speed (degrees/sec)
    float m_rotationTime = 0.0f; // Rotation time (seconds)

    static constexpr float GRAVITY = 9.81f; // Gravity constant

    // Grid zone definition constants
    static constexpr float GRID_Z_POSITION = -3.5f; // Z position of the grid
    static constexpr float GRID_Y_POSITION = 2.0f; // Y position of the grid
    static constexpr float GRID_THICKNESS = 3.0f; // Thickness of the grid
    static constexpr float COLLISION_THRESHOLD = 0.5f; // Collision threshold

    Game *m_game = nullptr; // Pointer to the game instance for scoring and life management

    // Shadow constants
    static constexpr float SHADOW_ALPHA = 0.5f; // Shadow transparency
    static constexpr float MIN_SHADOW_SCALE = 0.5f; // Minimum shadow scale
    static constexpr float MAX_SHADOW_SCALE = 1.2f; // Maximum shadow scale

};

#endif // PROJECTILE_H
