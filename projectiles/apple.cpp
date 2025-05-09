#include <GL/gl.h>
#include <GL/glu.h>
#include "apple.h"
#include "../projectileManager.h"
#include "appleHalf.h"
#include <QOpenGLTexture>
#include <QImage>

static QOpenGLTexture *g_appleTexture = nullptr;

Apple::Apple(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ)
{
    // Apple-specific initialization if needed
    if (!g_appleTexture)
        g_appleTexture = new QOpenGLTexture(QImage(":/apple_color.jpg").mirrored());
}

void Apple::draw()
{
    if (!isActive())
        return;

    glEnable(GL_LIGHTING);

    // Save current state
    glPushMatrix();

    // Position the sphere at the projectile's position
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    // Rotate texture by 90° on X axis
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    // Activate texture
    if (g_appleTexture) {
        glEnable(GL_TEXTURE_2D);
        g_appleTexture->bind();
    }
    // Ensure color is white
    glColor3f(1.0f, 1.0f, 1.0f);

    // Draw a larger sphere
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricTexture(quadric, GL_TRUE);
    gluSphere(quadric, RADIUS, 20, 20);
    gluDeleteQuadric(quadric);

    if (g_appleTexture) {
        g_appleTexture->release();
        glDisable(GL_TEXTURE_2D);
    }

    // Restore previous state
    glPopMatrix();

    // Re-enable lighting after drawing
    glEnable(GL_LIGHTING);

    // Reset color to white for subsequent objects
    glColor3f(1.0f, 1.0f, 1.0f);
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
