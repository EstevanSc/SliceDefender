#ifndef BANANAHALF_H
#define BANANAHALF_H

#include "../projectile.h"
#include <QOpenGLTexture>

/**
 * @brief Represents a half of a banana after slicing.
 * Handles its own drawing, physics update, and prevents further slicing.
 * @author Aubin SIONVILLE
 */
class ProjectileManager;

class BananaHalf : public Projectile
{
public:
    /**
     * @brief Enum for the type of half (front or back).
     */
    enum class HalfType
    {
        FRONT, // Front half of the banana
        BACK   // Back half of the banana
    };

    /**
     * @brief Constructor for BananaHalf.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     * @param type Type of half (FRONT or BACK)
     */
    BananaHalf(float startX, float startY, float startZ,
               float velocityX, float velocityY, float velocityZ,
               HalfType type);

    /**
     * @brief Destructor for BananaHalf
     */
    ~BananaHalf() override = default;

    /**
     * @brief Draws the banana half using OpenGL.
     * The half is drawn as a textured mesh with a clipping plane, mimicking the banana shape.
     */
    void draw() override;

    /**
     * @brief Halves cannot be sliced, does nothing.
     * @param manager Unused (not used for halves)
     * @note This method is overridden to prevent slicing of the halves.
     */
    void slice(ProjectileManager*) override;

    /**
     * @brief Returns the radius of the banana half.
     * @return Radius as float
     */
    float getRadius() const override;

    /**
     * @brief Updates the physics of the banana half.
     * @param deltaTime Time step
     */
    void update(float deltaTime) override;

    static constexpr int SEGMENTS = 8;
    static constexpr int SIDES = 10;
    static constexpr float HALF_LENGTH = 0.6f;
    static constexpr float RADIUS = 0.15f;
    static constexpr float CURVE_ANGLE = 55.0f;

private:
    HalfType m_type; // Type of half (FRONT or BACK)
    static QOpenGLTexture *s_bananaTexture; // Static texture to use on all instances
};

#endif // BANANAHALF_H
