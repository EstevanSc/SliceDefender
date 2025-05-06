#ifndef ORANGE_H
#define ORANGE_H

#include "../projectile.h"

class ProjectileManager;

class Orange : public Projectile
{
public:
    Orange(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    ~Orange() override = default;

    void draw() override;
    void slice(ProjectileManager *manager) override;
    float getRadius() const override;
    void getColor(float &r, float &g, float &b) const override;

private:
    // Sphere radius for the orange
    static constexpr float RADIUS = 0.25f;
};

#endif // ORANGE_H
