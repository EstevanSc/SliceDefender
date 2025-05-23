#ifndef STRAWBERRYHALF_H
#define STRAWBERRYHALF_H

#include "../projectile.h"

/**
 * @brief Represents a half of a strawberry after slicing.
 * Handles its own drawing, physics update, and prevents further slicing.
 * @author Aubin SIONVILLE
 */
class StrawberryHalf : public Projectile
{
public:
    /**
     * @brief Enum for the type of half (left or right).
     */
    enum class HalfType
    {
        LEFT, // Left half of the strawberry
        RIGHT // Right half of the strawberry
    };

    /**
     * @brief Constructor for StrawberryHalf.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     * @param type Type of half (LEFT or RIGHT)
     */
    StrawberryHalf(float startX, float startY, float startZ,
                   float velocityX, float velocityY, float velocityZ,
                   HalfType type);
    ~StrawberryHalf() override = default;

    /**
     * @brief Draws the strawberry half using OpenGL.
     * The half is constructed as a textured mesh with a clipping plane, a green calyx (circle), and leaves at the top.
     */
    void draw() override;

    /**
     * @brief Halves cannot be sliced, does nothing.
     * @param manager Unused (not used for halves)
     * @note This method is overridden to prevent slicing of the halves.
     */
    void slice(ProjectileManager*) override;

    /**
     * @brief Returns the radius of the strawberry half.
     * @return Radius as float
     */
    float getRadius() const override;

    /**
     * @brief Updates the physics of the strawberry half.
     * @param deltaTime Time step
     */
    void update(float deltaTime) override;

private:
    HalfType m_type;
    static constexpr float RADIUS = 0.25f;
};

#endif // STRAWBERRYHALF_H
