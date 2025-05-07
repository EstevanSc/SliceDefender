#ifndef CANNON_H
#define CANNON_H

#include <GL/glu.h>
#include <QVector3D>
#include <QColor>

/**
 * @class Cannon
 * @brief Represents a cannon that fires projectiles in the game
 *
 * This class manages the 3D cannon model including:
 * - The main cannon tube
 * - Decorative elements like rings and reinforcements
 * - The supporting wheels and axle
 */
class Cannon
{
public:
    /**
     * @brief Constructor initializes cannon with default parameters
     */
    Cannon();

    /**
     * @brief Destructor
     */
    ~Cannon();

    // Configuration methods
    /**
     * @brief Set the cannon's position in 3D space
     * @param pos Position vector
     */
    void setPosition(const QVector3D &pos);

    /**
     * @brief Set the cannon's elevation angle in degrees
     * @param angleDegrees Angle in degrees (positive = upward)
     */
    void setAngle(float angleDegrees);

    /**
     * @brief Set the cannon's barrel length
     * @param length Length value
     */
    void setLength(float length);

    /**
     * @brief Set the cannon's barrel radius
     * @param radius Radius value
     */
    void setRadius(float radius);

    /**
     * @brief Set the cannon's primary color
     * @param color QColor object
     */
    void setColor(const QColor &color);

    /**
     * @brief Set the tessellation quality for cylindrical parts
     * @param segments Number of segments
     */
    void setSegments(int segments);

    // Wheel parameters
    /**
     * @brief Set the cannon's wheel radius
     * @param radius Wheel radius value
     */
    void setWheelRadius(float radius);

    /**
     * @brief Set the cannon's wheel thickness
     * @param thickness Wheel thickness value
     */
    void setWheelThickness(float thickness);

    /**
     * @brief Set the length of the axle connecting the wheels
     * @param length Axle length value
     */
    void setAxisLength(float length);

    // Accessor methods
    /**
     * @brief Get the cannon's position
     * @return Position as QVector3D
     */
    QVector3D getPosition() const;

    /**
     * @brief Get the cannon's angle
     * @return Angle in degrees
     */
    float getAngle() const;

    /**
     * @brief Get the cannon's barrel length
     * @return Length value
     */
    float getLength() const;

    /**
     * @brief Get the cannon's barrel radius
     * @return Radius value
     */
    float getRadius() const;

    /**
     * @brief Get the cannon's color
     * @return Color as QColor
     */
    QColor getColor() const;

    /**
     * @brief Get the tessellation quality
     * @return Number of segments
     */
    int getSegments() const;

    /**
     * @brief Get the wheel radius
     * @return Wheel radius value
     */
    float getWheelRadius() const;

    /**
     * @brief Get the wheel thickness
     * @return Wheel thickness value
     */
    float getWheelThickness() const;

    /**
     * @brief Get the axle length
     * @return Axle length value
     */
    float getAxisLength() const;

    /**
     * @brief Check if low-poly mode is active
     * @return True if low-poly mode is active
     */
    bool isLowPoly() const;

    /**
     * @brief Render the cannon in 3D
     */
    void draw() const;

private:
    // Helper drawing methods
    /**
     * @brief Draw the main cannon tube with decorative elements
     */
    void drawCannonTube() const;

    /**
     * @brief Draw the wheels and axle
     */
    void drawWheels() const;

    /**
     * @brief Draw structural reinforcements
     */
    void drawReinforcements() const;

    /**
     * @brief Draw additional decorative details
     */
    void drawDetails() const;

    // Utility methods
    /**
     * @brief Draw a decorative ring around the cannon
     * @param quadric GLU quadric object for drawing
     * @param distance Distance from the base of the cannon
     * @param ringWidth Width of the ring
     * @param scale Radius scale factor relative to the cannon radius
     */
    void drawRing(GLUquadric *quadric, float distance, float ringWidth, float scale = 1.05f) const;

    /**
     * @brief Draw a wheel at the specified offset
     * @param quadric GLU quadric object for drawing
     * @param xOffset X-axis offset from the cannon center
     */
    void drawWheel(GLUquadric *quadric, float xOffset) const;

    // Member variables
    QVector3D m_position;
    float m_angleDegrees;
    float m_length;
    float m_radius;
    QColor m_color;
    int m_segments;

    // Wheel properties
    float m_wheelRadius;
    float m_wheelThickness;
    float m_axisLength;
};

#endif // CANNON_H
