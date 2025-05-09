#ifndef BANANA_H
#define BANANA_H

#include "../projectile.h"
#include "bananaHalf.h"

class ProjectileManager;

class Banana : public Projectile
{
public:
    Banana(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    ~Banana() override = default;

    void draw() override;
    void slice(ProjectileManager *manager) override;
    float getRadius() const override;
    void getColor(float &r, float &g, float &b) const override;

    static constexpr int SEGMENTS = 16;
    static constexpr int SIDES = 10;
    static constexpr float LENGTH = 1.2f;
    static constexpr float RADIUS = 0.15f;
    static constexpr float CURVE_ANGLE = 110.0f;
};

#endif // BANANA_H
