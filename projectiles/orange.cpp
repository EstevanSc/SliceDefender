#include <GL/gl.h>
#include <GL/glu.h>
#include "orange.h"
#include "../projectileManager.h"
#include "orangeHalf.h"
#include <QOpenGLTexture>
#include <QImage>

static QOpenGLTexture *g_orangeTexture = nullptr;

Orange::Orange(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ)
{
    if (!g_orangeTexture)
        g_orangeTexture = new QOpenGLTexture(QImage(":/orange_color.jpg").mirrored());
}

void Orange::draw()
{
    if (!isActive())
        return;

    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    float angle = m_rotationSpeed * m_rotationTime;
    glRotatef(angle, m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    // Activate texture
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

    glPopMatrix();

    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
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

    leftHalf->setRotationAxis(m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    leftHalf->setRotationSpeed(m_rotationSpeed);
    leftHalf->setRotationTime(m_rotationTime);
    rightHalf->setRotationAxis(m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    rightHalf->setRotationSpeed(m_rotationSpeed);
    rightHalf->setRotationTime(m_rotationTime);

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
