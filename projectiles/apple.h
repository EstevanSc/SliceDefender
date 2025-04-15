#ifndef APPLE_H
#define APPLE_H

#include "../projectile.h"

class Apple : public Projectile
{
public:
    Apple(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ);
    ~Apple() override = default;

    void draw() override;

private:
    // Sphere radius for the apple
    static constexpr float RADIUS = 0.25f;
};

#endif // APPLE_H
