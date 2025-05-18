#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <vector>
#include <QVector3D>

class ProjectileManager; // Forward declaration
class Game;              // Forward declaration
class Player;            // Forward declaration

class Projectile
{
public:
    Projectile(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    virtual ~Projectile() = default;

    virtual void draw() = 0;
    virtual void update(float deltaTime);

    // Slice-related methods
    virtual void slice(ProjectileManager *manager) = 0;
    bool shouldSlice() const;
    void setSliced();
    void setShouldSlice(bool value);
    virtual float getRadius() const = 0;
    virtual void getColor(float &r, float &g, float &b) const = 0;

    // Collision detection methods
    bool isNearPlayer(const Player &player) const;
    bool isInGridZone(float gridZPosition, float gridYPosition, float gridThickness) const;
    bool isInGridZoneAndPassed(float gridZPosition, float gridYPosition, float gridThickness) const;
    bool hasTouchedFloor() const;
    void checkCollisionWithPlayer(Player *player);

    // Getters
    float *getPosition() const;
    float *getVelocity() const;
    float *getAcceleration() const;

    // Setters
    void setPosition(float x, float y, float z);
    void setVelocity(float vx, float vy, float vz);
    void setAcceleration(float ax, float ay, float az);
    void setGame(Game *game) { m_game = game; }

    bool isActive() const;
    void setActive(bool active);

    virtual bool isHalf() const { return false; }

    // Life loss tracking
    bool hasDecreasedLife() const { return m_hasDecreasedLife; }
    void setDecreasedLife(bool decreased) { m_hasDecreasedLife = decreased; }

    // Shadow rendering method
    void drawShadow() const;

protected:
    float m_position[3];
    float m_velocity[3];
    float m_acceleration[3];
    bool m_isActive;
    bool m_sliced = false;
    bool m_shouldSlice = false;
    bool m_hasDecreasedLife = false; // Flag to track if projectile has already caused life loss

    // Grid zone definition constants
    static constexpr float GRID_Z_POSITION = -3.5f;    // Z position of cylindrical grid
    static constexpr float GRID_Y_POSITION = 2.0f;     // Y position of grid (height)
    static constexpr float GRID_THICKNESS = 3.0f;      // Collision detection zone thickness
    static constexpr float COLLISION_THRESHOLD = 0.3f; // Sword collision detection threshold
    static constexpr float GRAVITY = 9.81f;            // Gravity constant

    Game *m_game = nullptr; // Pointer to the game instance for scoring and life management

    // Shadow constants
    static constexpr float SHADOW_ALPHA = 0.5f;     // Shadow transparency
    static constexpr float MIN_SHADOW_SCALE = 0.5f; // Minimum shadow scale factor
    static constexpr float MAX_SHADOW_SCALE = 1.2f; // Maximum shadow scale factor

    // Helper method for shadow calculations
    float calculateShadowScale() const;
};

#endif // PROJECTILE_H
