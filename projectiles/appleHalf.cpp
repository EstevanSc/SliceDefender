#include <GL/gl.h>
#include <GL/glu.h>
#include "appleHalf.h"
#include "../projectileManager.h"
#include <QOpenGLTexture>
#include <QImage>
#include <QQuaternion>
#include <QVector3D>

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

    // Cutting plane (for slicing)
    float angle = m_rotationSpeed * m_rotationTime;
    QVector3D baseNormal((m_type == HalfType::LEFT) ? 1.0f : -1.0f, 0.0f, 0.0f);
    QVector3D axis(m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);
    QVector3D rotatedNormal = q.rotatedVector(baseNormal);

    double planeEq[4] = {rotatedNormal.x(), rotatedNormal.y(), rotatedNormal.z(), 0.0};
    glClipPlane(GL_CLIP_PLANE0, planeEq);
    glEnable(GL_CLIP_PLANE0);

    glRotatef(angle, m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
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

    // Deactivate only if the half-apple goes off-screen or hits the ground
    if (m_position[1] <= 0.0f || m_position[2] >= 0.0f || m_position[2] <= -30.0f)
    {
        m_isActive = false;
    }

    // Important: do not activate shouldSlice for halves
}

void AppleHalf::slice(ProjectileManager*)
{
    // Halves cannot be sliced
    // Do nothing
}

float AppleHalf::getRadius() const
{
    return RADIUS;
}
