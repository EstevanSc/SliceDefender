#ifndef APPLEHALF_H
#define APPLEHALF_H

#include "../projectile.h"

/**
 * @brief Represents a half of an apple after slicing.
 * Handles its own drawing, physics update, and prevents further slicing.
 * @author Aubin SIONVILLE
 */
class AppleHalf : public Projectile
{
public:
    /**
     * @brief Enum for the type of half (left or right).
     */
    enum class HalfType
    {
        LEFT, // Left half of the apple
        RIGHT // Right half of the apple
    };

    /**
     * @brief Constructor for AppleHalf.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     * @param type Type of half (LEFT or RIGHT)
     */
    AppleHalf(float startX, float startY, float startZ,
              float velocityX, float velocityY, float velocityZ,
              HalfType type);

    /**
     * @brief Destructor for AppleHalf.
     */
    ~AppleHalf() override = default;

    /**
     * @brief Draws the apple half using OpenGL.
     * The half is rendered as a textured sphere with a clipping plane to show only one side.
     */
    void draw() override;

    /**
     * @brief Halves cannot be sliced, does nothing.
     * @param manager Unused (not used for halves)
     * @note This method is overridden to prevent slicing of the halves.
     */
    void slice(ProjectileManager *manager) override;

    /**
     * @brief Returns the radius of the apple half.
     * @return Radius as float
     */
    float getRadius() const override;

    /**
     * @brief Updates the physics of the apple half.
     * @param deltaTime Time step
     */
    void update(float deltaTime) override;

    /**
     * @brief Indicates this is a half projectile.
     * @return Always true
     */
    bool isHalf() const override { return true; }

private:
    HalfType m_type; // Type of half (LEFT or RIGHT)
    static constexpr float RADIUS = 0.25f; // Radius of the apple half
};

#endif // APPLEHALF_H
