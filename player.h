#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glu.h>
#include <QVector3D>
#include <QColor>

/**
 * @class Player
 * @brief Represents the player's sword in the game
 *
 * This class manages the player's sword which consists of:
 * - A cylindrical handle
 * - A rectangular guard (yellow)
 * - A blade (rectangular with a pyramid tip)
 *
 * The sword is positioned vertically and oriented to cut projectiles
 * coming from the cannon.
 * 
 * @author Estevan SCHMITT
 */
class Player
{
public:
    /**
     * @brief Constructor initializes the sword with default parameters
     */
    Player();

    /**
     * @brief Destructor
     */
    ~Player();

    /**
     * @brief Sets the position of the sword
     * @param pos The new position vector
     */
    void setPosition(const QVector3D &pos);

    /**
     * @brief Gets the current position of the sword
     * @return Current position as QVector3D
     */
    QVector3D getPosition() const;

    /**
     * @brief Sets the rotation of the sword
     * @param angleX Rotation angle around X-axis (in degrees)
     * @param angleY Rotation angle around Y-axis (in degrees)
     * @param angleZ Rotation angle around Z-axis (in degrees)
     */
    void setRotation(float angleX, float angleY, float angleZ);

    /**
     * @brief Gets the current rotation angles of the sword
     * @return QVector3D containing rotation angles (X, Y, Z) in degrees
     */
    QVector3D getRotation() const;

    /**
     * @brief Gets the blade tip position for more precise collision detection
     * @return QVector3D containing the world position of the blade tip
     */
    QVector3D getBladeTipPosition() const;

    /**
     * @brief Draws the sword with all its components
     */
    void draw() const;

private:
    // Helper methods for drawing sword components
    void drawHandle() const;
    void drawGuard() const;
    void drawBlade() const;
    void drawBladeTip() const;

    // Helper method for drawing decorative rings (similar to cannon)
    void drawRing(GLUquadric *quadric, float position, float width, float radiusMultiplier) const;

    // Position and rotation
    QVector3D m_position;
    QVector3D m_rotation; // X, Y, Z rotation angles in degrees

    // Sword dimensions
    float m_handleLength;
    float m_handleRadius;
    float m_guardWidth;
    float m_guardHeight;
    float m_guardDepth;
    float m_bladeLength;
    float m_bladeWidth;
    float m_bladeThickness;
    float m_tipLength;

    // Drawing quality
    int m_segments;

    // Colors
    QColor m_handleColor;
    QColor m_guardColor;
    QColor m_bladeColor;
};

#endif // PLAYER_H
