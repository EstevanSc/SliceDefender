#include <GL/gl.h>
#include <GL/glu.h>
#include "apple.h"

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
