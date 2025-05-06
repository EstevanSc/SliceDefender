#ifndef CORN_H
#define CORN_H

#include "../projectile.h"

class ProjectileManager;

class Corn : public Projectile
{
public:
    Corn(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    ~Corn() override = default;

    void draw() override;
    void slice(ProjectileManager *manager) override;
    float getRadius() const override;
    void getColor(float &r, float &g, float &b) const override;

private:
    static constexpr float RADIUS = 0.18f;
    static constexpr float LENGTH = 0.7f;
};

#endif // CORN_H
