#include <GL/gl.h>
#include <GL/glu.h>
#include "orangeHalf.h"
#include "../projectileManager.h"
#include <QOpenGLTexture>
#include <QImage>

static QOpenGLTexture *g_orangeTexture = nullptr;

OrangeHalf::OrangeHalf(float startX, float startY, float startZ,
                       float velocityX, float velocityY, float velocityZ,
                       HalfType type)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ),
      m_type(type)
{
    if (!g_orangeTexture)
        g_orangeTexture = new QOpenGLTexture(QImage(":/orange_color.jpg").mirrored());
}

void OrangeHalf::draw()
{
    if (!isActive())
        return;

    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    // Clipping for half-sphere
    double planeEq[4] = {0.0, 0.0, 0.0, 0.0};
    planeEq[0] = (m_type == HalfType::LEFT) ? 1.0 : -1.0;

    glClipPlane(GL_CLIP_PLANE0, planeEq);
    glEnable(GL_CLIP_PLANE0);

    // Rotate texture by 90Â° on X axis
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    // Texture
    if (g_orangeTexture) {
        glEnable(GL_TEXTURE_2D);
        g_orangeTexture->bind();
    }
    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricTexture(quadric, GL_TRUE);
    gluSphere(quadric, RADIUS, 20, 20);
    gluDeleteQuadric(quadric);

    if (g_orangeTexture) {
        g_orangeTexture->release();
        glDisable(GL_TEXTURE_2D);
    }

    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
}

// Override update to prevent halves from being sliced
void OrangeHalf::update(float deltaTime)
{
    // Update physics
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

    // Désactiver seulement si la demi-orange sort des limites du corridor
    // On ne désactive plus pour y <= 0 ici, cela est géré dans Game::checkCollisions
    if (m_position[2] >= 0.0f || m_position[2] <= -30.0f ||
        m_position[0] < -5.0f || m_position[0] > 5.0f)
    {
        m_isActive = false;
    }

    // Important: ne pas activer shouldSlice pour les moitiés
}

void OrangeHalf::slice(ProjectileManager *manager)
{
    // Halves cannot be sliced
    // Do nothing
}

float OrangeHalf::getRadius() const
{
    return RADIUS;
}

void OrangeHalf::getColor(float &r, float &g, float &b) const
{
    r = 1.0f;
    g = 0.5f;
    b = 0.0f; // Orange color
}
