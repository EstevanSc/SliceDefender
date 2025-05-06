#ifndef ORANGEHALF_H
#define ORANGEHALF_H

#include "../projectile.h"

class OrangeHalf : public Projectile
{
public:
    enum class HalfType
    {
        LEFT,
        RIGHT
    };

    OrangeHalf(float startX, float startY, float startZ,
               float velocityX, float velocityY, float velocityZ,
               HalfType type);
    ~OrangeHalf() override = default;

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

#endif // ORANGEHALF_H
