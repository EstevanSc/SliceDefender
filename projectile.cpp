#include "projectile.h"
#include <cmath>
#include <iostream>
#include <QTime>
#include <QVector3D>
#include "player.h"
#include "game.h"

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
    // Skip update if projectile is inactive
    if (!m_isActive)
    {
        return;
    }

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
        return;
    }

    // Check for collision with the player or other conditions
    // but only if game is running and we have a valid game instance
    if (m_game && m_game->isGameStarted())
    {
        // Check for collision with ground
        if (hasTouchedFloor())
        {
            // Only whole projectiles cause life loss, and only if they haven't already decreased a life
            if (!isHalf() && !m_hasDecreasedLife)
            {
                m_hasDecreasedLife = true;
                m_game->loseLife();
            }
            m_isActive = false;
            return;
        }

        // Check for collision with grid zone
        if (isInGridZoneAndPassed(GRID_Z_POSITION, GRID_Y_POSITION, GRID_THICKNESS))
        {
            // Only whole projectiles cause life loss if not sliced when passing through grid
            if (!isHalf() && !m_hasDecreasedLife)
            {
                m_hasDecreasedLife = true;
                m_game->loseLife();
            }
            return;
        }

        // Check for collision with player's sword
        if (m_game->getPlayer())
        {
            checkCollisionWithPlayer(m_game->getPlayer());
        }
    }
}

/**
 * @brief Check if the projectile is near the player's sword
 * @param player The player object to check distance against
 * @return true if the projectile is within collision range of the player's sword
 */
bool Projectile::isNearPlayer(const Player &player) const
{
    // Get player's blade position for more accurate collision detection
    QVector3D swordBladePos = player.getBladeTipPosition();

    // Get projectile position
    QVector3D projectilePos(m_position[0], m_position[1], m_position[2]);

    // Calculate distance between projectile and sword blade
    float distance = (projectilePos - swordBladePos).length();

    // Check if within collision threshold (player's sword + projectile radius)
    return (distance < COLLISION_THRESHOLD + getRadius());
}

/**
 * @brief Check if the projectile is in the grid zone
 * @param gridZPosition Z position of the cylindrical grid
 * @param gridYPosition Y position of the cylindrical grid (height)
 * @param gridThickness Thickness of the grid for collision detection
 * @return true if the projectile is within the grid zone
 */
bool Projectile::isInGridZone(float gridZPosition, float gridYPosition, float gridThickness) const
{
    // Check if projectile is in the cylindrical grid zone
    return (m_position[2] >= gridZPosition - gridThickness &&
            m_position[2] <= gridZPosition + gridThickness &&
            std::abs(m_position[1] - gridYPosition) <= gridThickness);
}

/**
 * @brief Check if the projectile is in the grid zone and has passed through it
 * @param gridZPosition Z position of the cylindrical grid
 * @param gridYPosition Y position of the cylindrical grid (height)
 * @param gridThickness Thickness of the grid for collision detection
 * @return true if the projectile has passed through the grid zone without being sliced
 */
bool Projectile::isInGridZoneAndPassed(float gridZPosition, float gridYPosition, float gridThickness) const
{
    // Check if projectile is in grid zone and has passed the central Z position
    return isInGridZone(gridZPosition, gridYPosition, gridThickness) &&
           m_position[2] > gridZPosition;
}

/**
 * @brief Check if the projectile has touched the floor
 * @return true if the projectile's Y position is at or below 0
 */
bool Projectile::hasTouchedFloor() const
{
    return m_position[1] <= 0.0f;
}

/**
 * @brief Check for collision with the player's sword and handle slicing
 * @param player Pointer to the player object
 */
void Projectile::checkCollisionWithPlayer(Player *player)
{
    // Skip if already sliced or player is null
    if (m_sliced || !player)
    {
        return;
    }

    // Check if the projectile is near the player's sword
    if (isNearPlayer(*player))
    {
        // Mark the projectile for slicing
        m_shouldSlice = true;

        // Only whole projectiles give points
        if (!isHalf())
        {
            // Increase the player's score
            if (m_game)
            {
                m_game->gainPoint();
            }
        }
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

/**
 * @brief Draws a shadow disk underneath the projectile
 *
 * The shadow is a simple transparent black disk that follows the projectile's
 * X and Z position but remains on the ground (Y=0). The shadow size is
 * dynamically calculated based on the projectile's height from the ground.
 */
void Projectile::drawShadow() const
{
    // Don't draw shadow for inactive projectiles
    if (!m_isActive)
    {
        return;
    }
    
    float shadowScale = calculateShadowScale();

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Position the shadow on the ground directly below the projectile
    glPushMatrix();
    glTranslatef(m_position[0], 0.01f, m_position[2]); 
    glScalef(shadowScale, 1.0f, shadowScale);
    float alphaFactor = std::max(0.2f, std::min(1.0f, 1.0f - m_position[1] / 10.0f));
    float finalAlpha = SHADOW_ALPHA * alphaFactor;
    glColor4f(0.0f, 0.0f, 0.0f, finalAlpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);
    const int segments = 16;
    const float radius = getRadius();
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glPopMatrix();
    glPopAttrib();
}

/**
 * @brief Calculates the shadow scale based on projectile height
 *
 * As the projectile gets closer to the ground, the shadow grows larger.
 * The scale varies between MIN_SHADOW_SCALE (furthest) and MAX_SHADOW_SCALE (closest).
 *
 * @return The scale factor to apply to the shadow radius
 */
float Projectile::calculateShadowScale() const
{
    const float MAX_HEIGHT = 5.0f;
    float heightFactor = 1.0f - std::min(m_position[1], MAX_HEIGHT) / MAX_HEIGHT;
    heightFactor = heightFactor * heightFactor;
    return MIN_SHADOW_SCALE + heightFactor * (MAX_SHADOW_SCALE - MIN_SHADOW_SCALE);
}
