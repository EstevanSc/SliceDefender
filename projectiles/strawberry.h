#ifndef STRAWBERRY_H
#define STRAWBERRY_H

#include "../projectile.h"
#include "strawberryHalf.h"

class ProjectileManager;

class Strawberry : public Projectile
{
public:
    Strawberry(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    ~Strawberry() override = default;

    void draw() override;
    void slice(ProjectileManager *manager) override;
    float getRadius() const override;
    void getColor(float &r, float &g, float &b) const override;

private:
    static constexpr float RADIUS = 0.25f;
    static constexpr float HEIGHT = 0.8f;
};

#endif // STRAWBERRY_H
