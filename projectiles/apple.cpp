#include <GL/gl.h>
#include <GL/glu.h>
#include "apple.h"
#include "../projectileManager.h"
#include "appleHalf.h"

Apple::Apple(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ)
{
    // Apple-specific initialization if needed
}

void Apple::draw()
{
    if (!isActive())
        return;

    // Disable lighting to ensure fruits are always visible
    glDisable(GL_LIGHTING);

    // Save current state
    glPushMatrix();

    // Position the sphere at the projectile's position
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    // Bright green color for apple with high saturation
    glColor3f(0.0f, 1.0f, 0.0f);

    // Draw a larger sphere
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluSphere(quadric, RADIUS, 20, 20);
    gluDeleteQuadric(quadric);

    // Restore previous state
    glPopMatrix();

    // Re-enable lighting after drawing
    glEnable(GL_LIGHTING);
}

void Apple::slice(ProjectileManager *manager)
{
    // We give a lateral impulse to the halves to ensure they separate clearly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> lateralVelocity(2.0f, 5.0f);

    // Create the left half of the apple with an impulse to the left (positive in X)
    float leftVelocityX = m_velocity[0] + lateralVelocity(gen);
    AppleHalf *leftHalf = new AppleHalf(
        m_position[0] + 0.1f, m_position[1], m_position[2], // Initial offset to avoid collision
        leftVelocityX, m_velocity[1], m_velocity[2],
        AppleHalf::HalfType::LEFT);

    // Create the right half of the apple with an impulse to the right (negative in X)
    float rightVelocityX = m_velocity[0] - lateralVelocity(gen);
    AppleHalf *rightHalf = new AppleHalf(
        m_position[0] - 0.1f, m_position[1], m_position[2], // Initial offset to avoid interpenetration
        rightVelocityX, m_velocity[1], m_velocity[2],
        AppleHalf::HalfType::RIGHT);

    // Add the halves to the projectile manager
    manager->addProjectile(leftHalf);
    manager->addProjectile(rightHalf);
}

float Apple::getRadius() const
{
    return RADIUS;
}

void Apple::getColor(float &r, float &g, float &b) const
{
    r = 0.0f;
    g = 1.0f;
    b = 0.0f;
}
