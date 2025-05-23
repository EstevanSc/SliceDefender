#ifndef CANNON_H
#define CANNON_H

#include <GL/glu.h>
#include <QVector3D>
#include <QColor>

/**
 * @class Cannon
 * @brief Represents a cannon that fires projectiles in the game.
 *
 * Manages the 3D cannon model, including:
 * - Main tube
 * - Decorative rings and reinforcements
 * - Supporting wheels and axle
 *
 * @author Aubin SIONVILLE
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

    /**
     * @brief Render the cannon in 3D using the auxiliary functions
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
    QVector3D m_position; // Cannon position in 3D space
    float m_angleDegrees; // Cannon elevation angle in degrees
    float m_length; // Length of the cannon barrel
    float m_radius; // Radius of the cannon barrel
    QColor m_color; // Color of the cannon
    int m_segments; // Number of segments for cylindrical parts

    // Wheel properties
    float m_wheelRadius; // Radius of the wheels
    float m_wheelThickness; // Thickness of the wheels
    float m_axisLength; // Length of the axle connecting the wheels
};

#endif // CANNON_H
