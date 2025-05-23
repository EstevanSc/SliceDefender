#ifndef ORANGE_H
#define ORANGE_H

#include "../projectile.h"
#include "../projectileManager.h"

/**
 * @brief Represents a whole orange projectile.
 * Handles drawing, slicing, and radius information.
 * @author Aubin SIONVILLE
 */
class Orange : public Projectile
{
public:
    /**
     * @brief Constructor for Orange.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     */
    Orange(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    
    /**
     * @brief Destructor for Orange.
     */
    ~Orange() override = default;

    /**
     * @brief Draws the orange using OpenGL.
     * The orange is rendered as a textured sphere.
     */
    void draw() override;

    /**
     * @brief Slices the orange into two halves and adds them to the manager.
     * @param manager ProjectileManager to add halves to
     */
    void slice(ProjectileManager *manager) override;

    /**
     * @brief Returns the radius of the orange.
     * @return Radius as float
     */
    float getRadius() const override;

private:
    static constexpr float RADIUS = 0.25f; // Radius of the orange
};

#endif // ORANGE_H
