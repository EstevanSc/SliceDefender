#include <GL/gl.h>
#include <GL/glu.h>
#include "orange.h"
#include "../projectileManager.h"
#include "orangeHalf.h"

Orange::Orange(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ)
{
    // Orange-specific initialization if needed
}

void Orange::draw()
{
    if (!isActive())
        return;

    // Disable lighting to ensure fruits are always visible
    glDisable(GL_LIGHTING);

    // Save current state
    glPushMatrix();

    // Position the sphere at the projectile's position
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    // Bright orange color with high saturation
    glColor3f(1.0f, 0.5f, 0.0f);

    // Draw a sphere
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluSphere(quadric, RADIUS, 20, 20);
    gluDeleteQuadric(quadric);

    // Restore previous state
    glPopMatrix();

    // Re-enable lighting after drawing
    glEnable(GL_LIGHTING);
}

void Orange::slice(ProjectileManager *manager)
{
    // We give a lateral impulse to the halves to ensure they separate clearly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> lateralVelocity(2.0f, 5.0f);

    // Create the left half of the apple with an impulse to the left (positive in X)
    float leftVelocityX = m_velocity[0] + lateralVelocity(gen);
    OrangeHalf *leftHalf = new OrangeHalf(
        m_position[0] + 0.1f, m_position[1], m_position[2], // Initial offset to avoid collision
        leftVelocityX, m_velocity[1], m_velocity[2],
        OrangeHalf::HalfType::LEFT);

    // Create the right half of the apple with an impulse to the right (negative in X)
    float rightVelocityX = m_velocity[0] - lateralVelocity(gen);
    OrangeHalf *rightHalf = new OrangeHalf(
        m_position[0] - 0.1f, m_position[1], m_position[2], // Initial offset to avoid interpenetration
        rightVelocityX, m_velocity[1], m_velocity[2],
        OrangeHalf::HalfType::RIGHT);

    // Add the halves to the projectile manager
    manager->addProjectile(leftHalf);
    manager->addProjectile(rightHalf);
}

float Orange::getRadius() const
{
    return RADIUS;
}

void Orange::getColor(float &r, float &g, float &b) const
{
    r = 1.0f;
    g = 0.5f;
    b = 0.0f;
}
