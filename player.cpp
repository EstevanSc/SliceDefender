#define _USE_MATH_DEFINES

#include <GL/glu.h>
#include "player.h"
#include <cmath>

Player::Player()
    : m_position(0.0f, 0.0f, 0.0f),
      m_rotation(0.0f, 0.0f, 0.0f),
      m_handleLength(0.5f / 3.0f),
      m_handleRadius(0.05f / 3.0f),
      m_guardWidth(0.3f / 3.0f),
      m_guardHeight(0.05f / 3.0f),
      m_guardDepth(0.15f / 3.0f),
      m_bladeLength(1.0f / 3.0f),
      m_bladeWidth(0.08f / 3.0f),
      m_bladeThickness(0.02f / 3.0f),
      m_tipLength(0.2f / 3.0f),
      m_segments(8),
      m_handleColor(QColor(100, 70, 40)),
      m_guardColor(QColor(255, 215, 0)),
      m_bladeColor(QColor(200, 200, 220))
{
}

Player::~Player()
{
}

void Player::setPosition(const QVector3D &pos)
{
    m_position = pos;
}

QVector3D Player::getPosition() const
{
    return m_position;
}

void Player::setRotation(float angleX, float angleY, float angleZ)
{
    m_rotation = QVector3D(angleX, angleY, angleZ);
}

QVector3D Player::getRotation() const
{
    return m_rotation;
}

void Player::draw() const
{
    glPushMatrix();

    // Position the sword
    glTranslatef(m_position.x(), m_position.y(), m_position.z());

    // Apply rotation from user-provided values
    glRotatef(m_rotation.x(), 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotation.y(), 0.0f, 1.0f, 0.0f);
    glRotatef(m_rotation.z(), 0.0f, 0.0f, 1.0f);

    // Add a 90� rotation around the Y-axis to align the guard with the X-axis
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Draw sword components
    drawHandle();
    drawGuard();
    drawBlade();
    drawBladeTip();

    // Restore matrix
    glPopMatrix();
}

void Player::drawHandle() const
{
    GLUquadric *quadric = gluNewQuadric();

    // Position for the handle (bottom part of the sword)
    glPushMatrix();

    // Rotate so the cylinder is vertical (along Y axis)
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    // Set the main handle color (dark brown)
    glColor3f(
        m_handleColor.redF() * 0.8f,
        m_handleColor.greenF() * 0.8f,
        m_handleColor.blueF() * 0.8f);

    // Draw the handle as a cylinder
    gluCylinder(quadric, m_handleRadius, m_handleRadius, m_handleLength, m_segments, 1);

    // Cap the bottom of the handle
    gluDisk(quadric, 0.0f, m_handleRadius, m_segments, 1);

    // Base pommel ring (wider, golden like the guard)
    glColor3f(
        m_guardColor.redF(),
        m_guardColor.greenF(),
        m_guardColor.blueF());

    // Bottom ring (pommel)
    float pommelRadius = m_handleRadius * 1.5f;
    float pommelHeight = m_handleLength * 0.1f;
    gluCylinder(quadric, pommelRadius, pommelRadius, pommelHeight, m_segments, 1);
    gluDisk(quadric, 0.0f, pommelRadius, m_segments, 1);

    glTranslatef(0.0f, 0.0f, pommelHeight);
    gluDisk(quadric, 0.0f, pommelRadius, m_segments, 1);

    // Middle part of the handle - leather wrapping texture effect
    glColor3f(
        m_handleColor.redF(),
        m_handleColor.greenF() * 0.8f,
        m_handleColor.blueF() * 0.6f); // Leather brown

    float mainHandleStart = pommelHeight;
    float mainHandleLength = m_handleLength * 0.8f;
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluCylinder(quadric, m_handleRadius * 1.1f, m_handleRadius * 1.1f, mainHandleLength, m_segments, 1);

    // Add texture effect with thin rings for leather wrapping
    glColor3f(
        m_handleColor.redF() * 0.7f,
        m_handleColor.greenF() * 0.5f,
        m_handleColor.blueF() * 0.3f); // Darker leather

    float wrapSpacing = mainHandleLength / 8.0f;
    for (int i = 0; i < 7; i++)
    {
        float wrapPosition = mainHandleStart + wrapSpacing * (i + 0.5f);
        float wrapWidth = wrapSpacing * 0.3f;

        glTranslatef(0.0f, 0.0f, wrapSpacing - wrapWidth);
        gluCylinder(quadric, m_handleRadius * 1.15f, m_handleRadius * 1.15f, wrapWidth, m_segments, 1);
    }

    // Top decorative ring near the guard
    glColor3f(
        m_guardColor.redF(),
        m_guardColor.greenF(),
        m_guardColor.blueF());

    float topRingHeight = m_handleLength * 0.1f;
    float topRingStart = m_handleLength - topRingHeight;

    glTranslatef(0.0f, 0.0f, mainHandleLength - (7 * wrapSpacing - 7 * wrapSpacing * 0.3f));
    gluCylinder(quadric, m_handleRadius * 1.3f, m_handleRadius * 1.3f, topRingHeight, m_segments, 1);
    gluDisk(quadric, m_handleRadius * 1.1f, m_handleRadius * 1.3f, m_segments, 1);

    // Cap the top of the handle (at the guard)
    glTranslatef(0.0f, 0.0f, topRingHeight);
    gluDisk(quadric, 0.0f, m_handleRadius * 1.3f, m_segments, 1);

    glPopMatrix();

    gluDeleteQuadric(quadric);
}

void Player::drawGuard() const
{
    // Set the guard color (yellow/gold)
    glColor3f(
        m_guardColor.redF(),
        m_guardColor.greenF(),
        m_guardColor.blueF());

    // Position for the guard (above the handle)
    glPushMatrix();
    glTranslatef(0.0f, m_handleLength, 0.0f);

    // Draw guard as a rectangular block
    glPushMatrix();
    glScalef(m_guardWidth, m_guardHeight, m_guardDepth);
    glTranslatef(-0.5f, 0.0f, -0.5f); // Center the guard

    // Front face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glEnd();

    // Back face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    // Top face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glEnd();

    // Bottom face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    // Left face
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    // Right face
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glEnd();

    glPopMatrix();
    glPopMatrix();
}

void Player::drawBlade() const
{
    // Set the blade color (silver)
    glColor3f(
        m_bladeColor.redF(),
        m_bladeColor.greenF(),
        m_bladeColor.blueF());

    // Position for the blade (above the guard)
    glPushMatrix();
    glTranslatef(0.0f, m_handleLength + m_guardHeight, 0.0f);

    // Draw blade as a rectangular block
    glPushMatrix();
    glScalef(m_bladeWidth, m_bladeLength, m_bladeThickness);
    glTranslatef(-0.5f, 0.0f, -0.5f); // Center the blade

    // Front face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glEnd();

    // Back face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    // Top face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glEnd();

    // Bottom face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    // Left face
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    // Right face
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glEnd();

    glPopMatrix();
    glPopMatrix();
}

void Player::drawBladeTip() const
{
    // Set the blade color (silver, same as blade)
    glColor3f(
        m_bladeColor.redF(),
        m_bladeColor.greenF(),
        m_bladeColor.blueF());

    // Position for the blade tip (above the blade)
    glPushMatrix();
    glTranslatef(0.0f, m_handleLength + m_guardHeight + m_bladeLength, 0.0f);

    // Draw blade tip as a pyramid
    glPushMatrix();

    // Pyramid base dimensions
    float halfWidth = m_bladeWidth / 2.0f;
    float halfThickness = m_bladeThickness / 2.0f;

    // Pyramid base vertices (centered on origin)
    float baseVertices[4][3] = {
        {-halfWidth, 0.0f, -halfThickness},
        {halfWidth, 0.0f, -halfThickness},
        {halfWidth, 0.0f, halfThickness},
        {-halfWidth, 0.0f, halfThickness}};

    // Draw pyramid
    glBegin(GL_TRIANGLES);

    // Front face
    glNormal3f(0.0f, m_tipLength, halfThickness);
    glVertex3f(0.0f, m_tipLength, 0.0f); // Tip
    glVertex3f(baseVertices[2][0], baseVertices[2][1], baseVertices[2][2]);
    glVertex3f(baseVertices[3][0], baseVertices[3][1], baseVertices[3][2]);

    // Back face
    glNormal3f(0.0f, m_tipLength, -halfThickness);
    glVertex3f(0.0f, m_tipLength, 0.0f); // Tip
    glVertex3f(baseVertices[0][0], baseVertices[0][1], baseVertices[0][2]);
    glVertex3f(baseVertices[1][0], baseVertices[1][1], baseVertices[1][2]);

    // Left face
    glNormal3f(-halfWidth, m_tipLength, 0.0f);
    glVertex3f(0.0f, m_tipLength, 0.0f); // Tip
    glVertex3f(baseVertices[3][0], baseVertices[3][1], baseVertices[3][2]);
    glVertex3f(baseVertices[0][0], baseVertices[0][1], baseVertices[0][2]);

    // Right face
    glNormal3f(halfWidth, m_tipLength, 0.0f);
    glVertex3f(0.0f, m_tipLength, 0.0f); // Tip
    glVertex3f(baseVertices[1][0], baseVertices[1][1], baseVertices[1][2]);
    glVertex3f(baseVertices[2][0], baseVertices[2][1], baseVertices[2][2]);

    glEnd();

    glPopMatrix();
    glPopMatrix();
}

/**
 * @brief Gets the position of the blade tip for more precise collision detection
 * @return Position of the blade tip in world coordinates
 */
QVector3D Player::getBladeTipPosition() const
{
    // Calculate the offset from the player position to the blade tip
    float totalHeight = m_handleLength + m_guardHeight + m_bladeLength + m_tipLength;

    // Start with the player position
    QVector3D bladeTip = m_position;

    // Apply rotations to calculate the blade tip position
    float rotX = m_rotation.x() * M_PI / 180.0f;
    float rotY = m_rotation.y() * M_PI / 180.0f;
    float rotZ = m_rotation.z() * M_PI / 180.0f;

    // Create a direction vector pointing along the blade
    QVector3D bladeDir(0.0f, totalHeight, 0.0f);

    // Apply Y rotation (most significant for sword orientation)
    float cosY = cos(rotY);
    float sinY = sin(rotY);
    float newX = bladeDir.x() * cosY - bladeDir.z() * sinY;
    float newZ = bladeDir.x() * sinY + bladeDir.z() * cosY;
    bladeDir.setX(newX);
    bladeDir.setZ(newZ);

    // Add the direction vector to the base position
    bladeTip += bladeDir;

    return bladeTip;
}
