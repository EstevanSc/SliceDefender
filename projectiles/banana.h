#ifndef BANANA_H
#define BANANA_H

#include "../projectile.h"
#include "bananaHalf.h"
#include "../projectileManager.h"

/**
 * @brief Represents a whole banana projectile.
 * Handles drawing, slicing, and radius information.
 * @author Aubin SIONVILLE
 */
class Banana : public Projectile
{
public:
    /**
     * @brief Constructor for Banana.
     * @param startX Initial X position
     * @param startY Initial Y position
     * @param startZ Initial Z position
     * @param velocityX Initial X velocity
     * @param velocityY Initial Y velocity
     * @param velocityZ Initial Z velocity
     */
    Banana(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    
    /**
     * @brief Destructor for Banana.
     */
    ~Banana() override = default;

    /**
     * @brief Draws the banana.
     * The banana is constructed vertex by vertex along an arc in the XY plane.
     * The thickness varies along the arc to mimic a real banana shape.
     * A banana peel texture is applied to the mesh, and brown caps are drawn at both ends.
     */
    void draw() override;

    /**
     * @brief Slices the banana into two halves and adds them to the manager.
     * @param manager ProjectileManager to add halves to
     */
    void slice(ProjectileManager *manager) override;

    /**
     * @brief Returns the radius of the banana.
     * @return Radius as float
     */
    float getRadius() const override;

    static constexpr int SEGMENTS = 16; // Number of segments for the banana arc
    static constexpr int SIDES = 10; // Number of sides for the banana arc
    static constexpr float LENGTH = 1.2f; // Length of the banana
    static constexpr float RADIUS = 0.15f; // Radius of the banana
    static constexpr float CURVE_ANGLE = 110.0f; // Angle of the banana curve
};

#endif // BANANA_H
