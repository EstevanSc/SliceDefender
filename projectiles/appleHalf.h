#ifndef APPLEHALF_H
#define APPLEHALF_H

#include "../projectile.h"

class AppleHalf : public Projectile
{
public:
    enum class HalfType
    {
        LEFT,
        RIGHT
    };

    AppleHalf(float startX, float startY, float startZ,
              float velocityX, float velocityY, float velocityZ,
              HalfType type);
    ~AppleHalf() override = default;

    void draw() override;

    // Halves cannot be sliced
    void slice(ProjectileManager *manager) override;

    float getRadius() const override;
    void getColor(float &r, float &g, float &b) const override;

    // Override to ignore the slicing area
    void update(float deltaTime) override;

private:
    HalfType m_type;
    static constexpr float RADIUS = 0.25f;
};

#endif // APPLEHALF_H
