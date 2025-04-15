#ifndef ORANGE_H
#define ORANGE_H

#include "../projectile.h"

class Orange : public Projectile
{
public:
    Orange(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    ~Orange() override = default;

    void draw() override;

private:
    // Sphere radius for the orange
    static constexpr float RADIUS = 0.25f;
};

#endif // ORANGE_H
