#include <GL/gl.h>
#include <GL/glu.h>
#include "corn.h"
#include "../projectileManager.h"
#include "cornHalf.h"
#include <QOpenGLTexture>
#include <QImage>
#include <QVector3D>
#include <QQuaternion>

static QOpenGLTexture *g_cornTexture = nullptr;

Corn::Corn(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ)
{
    if (!g_cornTexture)
        g_cornTexture = new QOpenGLTexture(QImage(":/corn_color.jpg").mirrored());
}

void Corn::draw()
{
    if (!isActive())
        return;

    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    // Rotate the corn over time
    float angle = m_rotationSpeed * m_rotationTime;
    glRotatef(angle, m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    if (g_cornTexture) {
        glEnable(GL_TEXTURE_2D);
        g_cornTexture->bind();
    }
    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricTexture(quadric, GL_TRUE);
    gluCylinder(quadric, RADIUS, RADIUS, LENGTH, 32, 1);

    // Draw the faces of the cylinder
    glPushMatrix();
    gluDisk(quadric, 0.0, RADIUS, 32, 1);
    glTranslatef(0.0f, 0.0f, LENGTH);
    gluDisk(quadric, 0.0, RADIUS, 32, 1);
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

void Corn::slice(ProjectileManager *manager)
{
    // Slicing according to the rotation plane
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> lateralVelocity(1.0f, 3.0f);

    // Calculate the normal of the cutting plane (perpendicular to the rotation axis, arbitrarily oriented)
    QVector3D axis(m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    float angle = m_rotationSpeed * m_rotationTime;
    // Take the base normal (X) and rotate it according to the axis and current angle
    QVector3D baseNormal(1.0f, 0.0f, 0.0f);
    QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);
    QVector3D normal = q.rotatedVector(baseNormal).normalized();

    // Offset to avoid overlap
    float offset = LENGTH / 4.0f;
    QVector3D pos(m_position[0], m_position[1], m_position[2]);
    QVector3D leftPos = pos - normal * offset;
    QVector3D rightPos = pos + normal * offset;

    // Lateral impulse to separate the halves
    float impulse = lateralVelocity(gen);
    QVector3D leftVel = QVector3D(m_velocity[0], m_velocity[1], m_velocity[2]) - normal * impulse;
    QVector3D rightVel = QVector3D(m_velocity[0], m_velocity[1], m_velocity[2]) + normal * impulse;

    // Create the left half of the corn with an impulse to the left
    CornHalf *leftHalf = new CornHalf(
        leftPos.x(), leftPos.y(), leftPos.z(),
        leftVel.x(), leftVel.y(), leftVel.z(),
        CornHalf::HalfType::FRONT);

    // Create the right half of the corn with an impulse to the right
    CornHalf *rightHalf = new CornHalf(
        rightPos.x(), rightPos.y(), rightPos.z(),
        rightVel.x(), rightVel.y(), rightVel.z(),
        CornHalf::HalfType::BACK);

    leftHalf->setRotationAxis(m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    leftHalf->setRotationSpeed(m_rotationSpeed);
    leftHalf->setRotationTime(m_rotationTime);
    rightHalf->setRotationAxis(m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    rightHalf->setRotationSpeed(m_rotationSpeed);
    rightHalf->setRotationTime(m_rotationTime);

    manager->addProjectile(leftHalf);
    manager->addProjectile(rightHalf);
}

float Corn::getRadius() const
{
    return RADIUS;
}