#include <GL/gl.h>
#include <GL/glu.h>
#include "appleHalf.h"
#include "../projectileManager.h"
#include <QOpenGLTexture>
#include <QImage>

static QOpenGLTexture *g_appleTexture = nullptr;

AppleHalf::AppleHalf(float startX, float startY, float startZ,
                     float velocityX, float velocityY, float velocityZ,
                     HalfType type)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ),
      m_type(type)
{
    if (!g_appleTexture)
        g_appleTexture = new QOpenGLTexture(QImage(":/apple_color.jpg").mirrored());
}

void AppleHalf::draw()
{
    if (!isActive())
        return;

    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    // Clipping for the half-sphere
    double planeEq[4] = {0.0, 0.0, 0.0, 0.0};
    planeEq[0] = (m_type == HalfType::LEFT) ? 1.0 : -1.0;

    glClipPlane(GL_CLIP_PLANE0, planeEq);
    glEnable(GL_CLIP_PLANE0);

    // Rotate texture by 90° on X axis
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    // Texture
    if (g_appleTexture) {
        glEnable(GL_TEXTURE_2D);
        g_appleTexture->bind();
    }
    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricTexture(quadric, GL_TRUE);
    gluSphere(quadric, RADIUS, 20, 20);
    gluDeleteQuadric(quadric);

    if (g_appleTexture) {
        g_appleTexture->release();
        glDisable(GL_TEXTURE_2D);
    }

    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
}

// Override to ignore the slicing of already sliced projectiles
void AppleHalf::update(float deltaTime)
{
    // Physics update
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

    // Désactiver seulement si la demi-pomme sort des limites du corridor
    // On ne désactive plus pour y <= 0 ici, cela est géré dans Game::checkCollisions
    if (m_position[1] <= 0.0f || m_position[2] >= 0.0f || m_position[2] <= -30.0f)
    {
        m_isActive = false;
    }

    // Important: do not activate shouldSlice for halves
}

void AppleHalf::slice(ProjectileManager *manager)
{
    // Halves cannot be sliced
    // Do nothing
}

float AppleHalf::getRadius() const
{
    return RADIUS;
}

void AppleHalf::getColor(float &r, float &g, float &b) const
{
    r = 0.0f;
    g = 1.0f;
    b = 0.0f; // Green for apple
}
