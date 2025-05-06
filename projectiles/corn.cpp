#include <GL/gl.h>
#include <GL/glu.h>
#include "corn.h"
#include "../projectileManager.h"
#include "cornHalf.h"
#include <QOpenGLTexture>
#include <QImage>

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
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Ajouté : rotation pour aligner le cylindre sur X

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
    // Left/right impulse to separate the halves (X axis)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> lateralVelocity(1.0f, 3.0f);

    // Offset to place the halves to the left and right
    float dx = LENGTH / 4.0f;

    // Left
    float leftVelocityX = m_velocity[0] - lateralVelocity(gen);
    CornHalf *leftHalf = new CornHalf(
        m_position[0] - dx, m_position[1], m_position[2],
        leftVelocityX, m_velocity[1], m_velocity[2],
        CornHalf::HalfType::FRONT);

    // Right
    float rightVelocityX = m_velocity[0] + lateralVelocity(gen);
    CornHalf *rightHalf = new CornHalf(
        m_position[0] + dx, m_position[1], m_position[2],
        rightVelocityX, m_velocity[1], m_velocity[2],
        CornHalf::HalfType::BACK);

    manager->addProjectile(leftHalf);
    manager->addProjectile(rightHalf);
}

float Corn::getRadius() const
{
    return RADIUS;
}

void Corn::getColor(float &r, float &g, float &b) const
{
    r = 1.0f;
    g = 1.0f;
    b = 0.0f;
}
