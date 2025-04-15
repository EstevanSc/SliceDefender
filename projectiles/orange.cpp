#include <GL/gl.h>
#include <GL/glu.h>
#include "orange.h"

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
