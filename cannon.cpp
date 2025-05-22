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
    // Active l'éclairage pour que le canon soit affecté par les lumières
    glEnable(GL_LIGHTING);
    // Active GL_COLOR_MATERIAL pour permettre l'utilisation de glColor* avec le matériau
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Save the current matrix
    glPushMatrix();

    // Position the cannon
    glTranslatef(m_position.x(), m_position.y(), m_position.z());

    // Rotate the cannon according to the specified angle
    glRotatef(m_angleDegrees, 1.0f, 0.0f, 0.0f);

    // Draw the different parts of the cannon
    drawCannonTube();
    drawWheels();

    glPopMatrix();
}

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

    glPushMatrix();

    // Offset to align the cannon with the wheels
    glTranslatef(0.0f, m_wheelRadius * 0.5f, 0.0f);

    // Matériau canon (gris foncé, réagit à la lumière)
    GLfloat diffuse[4] = {50.0f/255.0f, 50.0f/255.0f, 50.0f/255.0f, 1.0f};
    GLfloat ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat specular[4] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat shininess = 20.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    // Draw the main tube
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluCylinder(quadric, m_radius, m_radius * 0.9f, m_length, m_segments, 1);

    // Bouche l'arrière du canon (z=0) avec une couleur "bois"
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(0.4f, 0.25f, 0.05f); // Couleur bois
    gluDisk(quadric, 0.0f, m_radius, m_segments, 1);
    glPopMatrix();

    // Copper/bronze color for the rings
    glColor3f(0.3f, 0.2f, 0.1f);

    // Decorative rings at different positions
    drawRing(quadric, 0.05f, 0.12f);
    drawRing(quadric, m_length * 0.5f, 0.08f);
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

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, m_length * 0.3f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Metallic axle
    glColor3f(0.3f, 0.2f, 0.1f); // Metallic color

    // Start of the axle at the edge of the cannon (m_radius)
    float axeStartPosition = -m_axisLength;

    glPopMatrix();

    // Left wheel
    drawWheel(quadric, -m_radius * 1.5f);

    // Right wheel
    drawWheel(quadric, m_radius * 1.2f);

    gluDeleteQuadric(quadric);
}