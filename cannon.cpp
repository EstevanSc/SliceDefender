#define _USE_MATH_DEFINES

#include <GL/glu.h>
#include "cannon.h"
#include <cmath>

Cannon::Cannon()
    : m_position(0.0f, 0.0f, 0.0f),
      m_angleDegrees(0.0f),
      m_length(2.0f),
      m_radius(0.2f),
      m_color(100, 100, 100),
      m_segments(8),
      m_wheelRadius(0.3f),
      m_wheelThickness(0.1f),
      m_axisLength(0.8f)
{
}

Cannon::~Cannon()
{
    // No resources to clean up
}

// Setters
void Cannon::setPosition(const QVector3D &pos) { m_position = pos; }
void Cannon::setAngle(float angleDegrees) { m_angleDegrees = angleDegrees; }
void Cannon::setLength(float length) { m_length = length; }
void Cannon::setRadius(float radius) { m_radius = radius; }
void Cannon::setColor(const QColor &color) { m_color = color; }
void Cannon::setSegments(int m_segments) { m_segments = m_segments; }
void Cannon::setWheelRadius(float radius) { m_wheelRadius = radius; }
void Cannon::setWheelThickness(float thickness) { m_wheelThickness = thickness; }
void Cannon::setAxisLength(float length) { m_axisLength = length; }

// Getters
QVector3D Cannon::getPosition() const { return m_position; }
float Cannon::getAngle() const { return m_angleDegrees; }
float Cannon::getLength() const { return m_length; }
float Cannon::getRadius() const { return m_radius; }
QColor Cannon::getColor() const { return m_color; }
int Cannon::getSegments() const { return m_segments; }
float Cannon::getWheelRadius() const { return m_wheelRadius; }
float Cannon::getWheelThickness() const { return m_wheelThickness; }
float Cannon::getAxisLength() const { return m_axisLength; }

void Cannon::draw() const
{
    // Save the current matrix
    glPushMatrix();

    // Position the cannon
    glTranslatef(m_position.x(), m_position.y(), m_position.z());

    // Rotate the cannon according to the specified angle
    glRotatef(m_angleDegrees, 1.0f, 0.0f, 0.0f);

    // Draw the different parts of the cannon
    drawCannonTube();
    drawWheels();

    // Restore the matrix
    glPopMatrix();
}

// Utility method to draw decorative rings around the cannon
void Cannon::drawRing(GLUquadric *quadric, float distance, float ringWidth, float ringDepth) const
{
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, distance);

    // Outer cylinder
    gluCylinder(quadric, m_radius * ringDepth, m_radius * ringDepth, ringWidth, m_segments, 1);

    // Front face of the ring
    gluDisk(quadric, m_radius, m_radius * ringDepth, m_segments, 1);

    // Back face of the ring
    glTranslatef(0.0f, 0.0f, ringWidth);
    gluDisk(quadric, m_radius, m_radius * ringDepth, m_segments, 1);

    glPopMatrix();
}

// Utility method to draw a wheel
void Cannon::drawWheel(GLUquadric *quadric, float xOffset) const
{
    glPushMatrix();
    glTranslatef(xOffset, 0.0f, m_length * 0.3f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Outer part of the wheel
    glColor3f(0.4f, 0.25f, 0.05f); // Dark wood
    gluCylinder(quadric, m_wheelRadius, m_wheelRadius, m_wheelThickness, m_segments, 1);

    // Wheel cylinder disks
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, m_wheelThickness);
    gluDisk(quadric, 0.0f, m_wheelRadius, m_segments, 1);
    glPopMatrix();
    glPushMatrix();
    gluDisk(quadric, 0.0f, m_wheelRadius, m_segments, 1);
    glPopMatrix();

    glPopMatrix();
}

void Cannon::drawCannonTube() const
{
    GLUquadric *quadric = gluNewQuadric();

    // Main cannon tube
    glPushMatrix();

    // Offset to align the cannon with the wheels
    glTranslatef(0.0f, m_wheelRadius * 0.5f, 0.0f);

    // Black color for the cannon tube
    glColor3f(0.1f, 0.1f, 0.1f);

    // Draw the main tube
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluCylinder(quadric, m_radius, m_radius * 0.9f, m_length, m_segments, 1);

    // Copper/bronze color for the rings
    glColor3f(0.3f, 0.2f, 0.1f);

    // Decorative rings at different positions
    // First ring near the base
    drawRing(quadric, 0.05f, 0.12f);

    // Ring at the middle of the cannon
    drawRing(quadric, m_length * 0.5f, 0.08f);

    // Ring near the end of the cannon
    drawRing(quadric, m_length - 0.15f, 0.08f);

    // Cannon mouth (interior, darker)
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, m_length);
    gluDisk(quadric, 0.0f, m_radius * 0.7f, m_segments, 1);
    glPopMatrix();

    glPopMatrix();

    gluDeleteQuadric(quadric);
}

void Cannon::drawWheels() const
{
    GLUquadric *quadric = gluNewQuadric();

    // Wheel axle (bar connecting the two wheels)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, m_length * 0.3f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Metallic axle
    glColor3f(0.3f, 0.2f, 0.1f); // Metallic color

    // Start of the axle at the edge of the cannon (m_radius)
    float axeStartPosition = -m_axisLength;

    glPopMatrix();

    // Left wheel - placed at the left edge of the cannon
    drawWheel(quadric, -m_radius * 1.5f);

    // Right wheel - placed at the right edge of the cannon
    drawWheel(quadric, m_radius * 1.2f);

    gluDeleteQuadric(quadric);
}