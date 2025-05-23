#ifndef CORN_H
#define CORN_H

#include "../projectile.h"
#include "../projectileManager.h"

/**
 * @brief Represents a whole corn cob projectile.
 * Handles drawing, slicing, and radius information.
 * @author Aubin SIONVILLE
 */
class Corn : public Projectile
{
public:
    /**
     * @brief Constructor for Corn.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     */
    Corn(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    
    /**
     * @brief Destructor for Corn.
     */
    ~Corn() override = default;

    /**
     * @brief Draws the corn using OpenGL.
     * The corn is rendered as a textured cylinder with disks at each end.
     */
    void draw() override;

    /**
     * @brief Slices the corn into two halves and adds them to the manager.
     * @param manager ProjectileManager to add halves to
     */
    void slice(ProjectileManager *manager) override;

    /**
     * @brief Returns the radius of the corn.
     * @return Radius as float
     */
    float getRadius() const override;

private:
    static constexpr float RADIUS = 0.18f; // Radius of the corn
    static constexpr float LENGTH = 0.7f; // Length of the corn
};

#endif // CORN_H
