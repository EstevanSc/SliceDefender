#ifndef STRAWBERRY_H
#define STRAWBERRY_H

#include "../projectile.h"
#include "strawberryHalf.h"
#include "../projectileManager.h"

/**
 * @brief Represents a whole strawberry projectile.
 * Handles drawing, slicing and radius information.
 * @author Aubin SIONVILLE
 */
class Strawberry : public Projectile
{
public:
    /**
     * @brief Constructor for Strawberry.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     */
    Strawberry(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    
    /**
     * @brief Destructor for Strawberry
     */
    ~Strawberry() override = default;

    /**
     * @brief Draws the strawberry using OpenGL.
     * The strawberry is constructed as a textured mesh with a green top and leaves at the top.
     * A strawberry texture is applied to the mesh.
     */
    void draw() override;

    /**
     * @brief Slices the strawberry into two halves and adds them to the manager.
     * @param manager ProjectileManager to add halves to
     */
    void slice(ProjectileManager *manager) override;

    /**
     * @brief Returns the radius of the strawberry.
     * @return Radius as float
     */
    float getRadius() const override;

private:
    static constexpr float RADIUS = 0.25f; // Radius of the strawberry
    static constexpr float HEIGHT = 0.8f; // Height of the strawberry
};

#endif // STRAWBERRY_H
