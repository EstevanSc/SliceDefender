#ifndef APPLE_H
#define APPLE_H

#include "../projectile.h"

/**
 * @brief Represents a whole apple projectile.
 * Handles drawing, slicing, and radius information.
 * @author Aubin SIONVILLE
 */
class ProjectileManager;

class Apple : public Projectile
{
public:
    /**
     * @brief Constructor for Apple.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     */
    Apple(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    
    /**
     * @brief Destructor for Apple.
     */
    ~Apple() override = default;

    /**
     * @brief Draws the apple as a textured sphere.
     */
    void draw() override;

    /**
     * @brief Slices the apple into two halves and adds them to the manager.
     * @param manager ProjectileManager to add halves to
     */
    void slice(ProjectileManager *manager) override;

    /**
     * @brief Returns the radius of the apple.
     * @return Radius as float
     */
    float getRadius() const override;

private:
    static constexpr float RADIUS = 0.25f;
};

#endif // APPLE_H
