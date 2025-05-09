#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <vector>

class ProjectileManager; // Forward declaration

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

    // Getters
    float *getPosition() const;
    float *getVelocity() const;
    float *getAcceleration() const;

    // Setters
    void setPosition(float x, float y, float z);
    void setVelocity(float vx, float vy, float vz);
    void setAcceleration(float ax, float ay, float az);

    bool isActive() const;
    void setActive(bool active);

    virtual bool isHalf() const { return false; }

    // Life loss tracking
    bool hasDecreasedLife() const { return m_hasDecreasedLife; }
    void setDecreasedLife(bool decreased) { m_hasDecreasedLife = decreased; }

protected:
    float m_position[3];
    float m_velocity[3];
    float m_acceleration[3];
    bool m_isActive;
    bool m_sliced = false;
    bool m_shouldSlice = false;
    bool m_hasDecreasedLife = false; // Flag to track if projectile has already caused life loss

    static constexpr float GRAVITY = 9.81f; // Gravity constant
};

#endif // PROJECTILE_H
