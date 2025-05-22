#ifndef CORN_HALF_H
#define CORN_HALF_H

#include "../projectile.h"

/**
 * @brief Represents a half of a corn cob after slicing.
 * Handles its own drawing, physics update, and prevents further slicing.
 * @author Aubin SIONVILLE
 */
class CornHalf : public Projectile
{
public:
    /**
     * @brief Enum for the type of half (front or back).
     */
    enum class HalfType
    {
        FRONT, // Front half of the corn
        BACK   // Back half of the corn
    };

    /**
     * @brief Constructor for CornHalf.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     * @param type Type of half (FRONT or BACK)
     */
    CornHalf(float startX, float startY, float startZ,
             float velocityX, float velocityY, float velocityZ,
             HalfType type);
    
    /**
     * @brief Destructor for CornHalf.
     */
    ~CornHalf() override = default;

    /**
     * @brief Draws the corn half using OpenGL.
     * The half is rendered as a textured cylinder with disks at each end.
     */
    void draw() override;

    /**
     * @brief Halves cannot be sliced, does nothing.
     * @param manager Unused (not used for halves)
     * @note This method is overridden to prevent slicing of the halves.
     */
    void slice(ProjectileManager *manager) override;

    /**
     * @brief Returns the radius of the corn half.
     * @return Radius as float
     */
    float getRadius() const override;

    /**
     * @brief Updates the physics of the corn half.
     * @param deltaTime Time step
     */
    void update(float deltaTime) override;

private:
    HalfType m_type;
    static constexpr float RADIUS = 0.18f;
    static constexpr float LENGTH = 0.35f;
};

#endif // CORN_HALF_H
