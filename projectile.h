#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <vector>

class Projectile
{
public:
    Projectile(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    virtual ~Projectile() = default;

    virtual void draw() = 0;
    virtual void update(float deltaTime);

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

protected:
    float m_position[3];
    float m_velocity[3];
    float m_acceleration[3];
    bool m_isActive;

    static constexpr float GRAVITY = 9.81f; // Gravity constant
};

#endif // PROJECTILE_H
