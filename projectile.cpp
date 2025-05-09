#include "projectile.h"
#include <cmath>
#include <iostream>
#include <QTime>

Projectile::Projectile(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ)
    : m_isActive(true), m_sliced(false), m_shouldSlice(false)
{
    m_position[0] = startX;
    m_position[1] = startY;
    m_position[2] = startZ;

    m_velocity[0] = velocityX;
    m_velocity[1] = velocityY;
    m_velocity[2] = velocityZ;

    m_acceleration[0] = 0.0f;
    m_acceleration[1] = -GRAVITY; // Y is up, so gravity is negative
    m_acceleration[2] = 0.0f;
}

void Projectile::update(float deltaTime)
{
    // Physics simulation using Euler implicit method

    // Reset acceleration each frame, with gravity applied to Y-axis
    m_acceleration[0] = 0.0f;
    m_acceleration[1] = -GRAVITY;
    m_acceleration[2] = 0.0f;

    // Update velocity based on acceleration
    m_velocity[0] += m_acceleration[0] * deltaTime;
    m_velocity[1] += m_acceleration[1] * deltaTime;
    m_velocity[2] += m_acceleration[2] * deltaTime;

    // Update position based on velocity
    m_position[0] += m_velocity[0] * deltaTime;
    m_position[1] += m_velocity[1] * deltaTime;
    m_position[2] += m_velocity[2] * deltaTime;

    // Deactivate projectile if it leaves the play area (corridor boundaries)
    // We only check X and Z limits here, Y limit (floor) is handled by collision detection
    if (m_position[1] <= 0.0f || m_position[2] >= 0.0f || m_position[2] <= -30.0f)
    {
        m_isActive = false;
    }
}

float *Projectile::getPosition() const
{
    return const_cast<float *>(m_position);
}

float *Projectile::getVelocity() const
{
    return const_cast<float *>(m_velocity);
}

float *Projectile::getAcceleration() const
{
    return const_cast<float *>(m_acceleration);
}

void Projectile::setPosition(float x, float y, float z)
{
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

void Projectile::setVelocity(float vx, float vy, float vz)
{
    m_velocity[0] = vx;
    m_velocity[1] = vy;
    m_velocity[2] = vz;
}

void Projectile::setAcceleration(float ax, float ay, float az)
{
    m_acceleration[0] = ax;
    m_acceleration[1] = ay;
    m_acceleration[2] = az;
}

bool Projectile::isActive() const
{
    return m_isActive;
}

void Projectile::setActive(bool active)
{
    m_isActive = active;
}

bool Projectile::shouldSlice() const
{
    bool should = m_shouldSlice && m_isActive;

    // Return the value and then reset it
    return should;
}

void Projectile::setSliced()
{
    // Mark as sliced and deactivate to avoid multiple slices
    m_shouldSlice = false;
    m_isActive = false;
}

void Projectile::setShouldSlice(bool value)
{
    m_shouldSlice = value;
}
