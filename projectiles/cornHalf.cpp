#include <GL/gl.h>
#include <GL/glu.h>
#include "cornHalf.h"
#include "../projectileManager.h"
#include <QOpenGLTexture>
#include <QImage>

static QOpenGLTexture *g_cornTexture = nullptr;

CornHalf::CornHalf(float startX, float startY, float startZ,
                   float velocityX, float velocityY, float velocityZ,
                   HalfType type)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ),
      m_type(type)
{
    if (!g_cornTexture)
        g_cornTexture = new QOpenGLTexture(QImage(":/corn_color.jpg").mirrored());
}

void CornHalf::draw()
{
    if (!isActive())
        return;

    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    // Apply the rotation of the corn half (inherited from the original corn)
    float angle = m_rotationSpeed * m_rotationTime;
    glRotatef(angle, m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Offset the corn half to match the correct half
    float offset = (m_type == HalfType::FRONT) ? -LENGTH / 2.0f : +LENGTH / 2.0f;
    glTranslatef(0.0f, 0.0f, offset);

    if (g_cornTexture) {
        glEnable(GL_TEXTURE_2D);
        g_cornTexture->bind();
    }
    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricTexture(quadric, GL_TRUE);
    gluCylinder(quadric, RADIUS, RADIUS, LENGTH, 32, 1);

    // Cylinder faces
    glPushMatrix();
    gluDisk(quadric, 0.0, RADIUS, 32, 1); // Front face
    glTranslatef(0.0f, 0.0f, LENGTH);
    gluDisk(quadric, 0.0, RADIUS, 32, 1); // Back face
    glPopMatrix();

    gluDeleteQuadric(quadric);

    if (g_cornTexture) {
        g_cornTexture->release();
        glDisable(GL_TEXTURE_2D);
    }

    glPopMatrix();

    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
}

void CornHalf::update(float deltaTime)
{
    m_acceleration[0] = 0.0f;
    m_acceleration[1] = -GRAVITY;
    m_acceleration[2] = 0.0f;

    m_velocity[0] += m_acceleration[0] * deltaTime;
    m_velocity[1] += m_acceleration[1] * deltaTime;
    m_velocity[2] += m_acceleration[2] * deltaTime;

    m_position[0] += m_velocity[0] * deltaTime;
    m_position[1] += m_velocity[1] * deltaTime;
    m_position[2] += m_velocity[2] * deltaTime;

    if (m_position[1] <= 0.0f || m_position[2] >= 0.0f || m_position[2] <= -30.0f)
    {
        m_isActive = false;
    }
}

void CornHalf::slice(ProjectileManager *manager)
{
    // Do nothing, halves cannot be sliced again
}

float CornHalf::getRadius() const
{
    return RADIUS;
}

void CornHalf::getColor(float &r, float &g, float &b) const
{
    r = 1.0f;
    g = 1.0f;
    b = 0.0f;
}
