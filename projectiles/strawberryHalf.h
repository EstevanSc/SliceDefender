#ifndef STRAWBERRYHALF_H
#define STRAWBERRYHALF_H

#include "../projectile.h"

class StrawberryHalf : public Projectile
{
public:
    enum class HalfType
    {
        LEFT,
        RIGHT
    };

    StrawberryHalf(float startX, float startY, float startZ,
                   float velocityX, float velocityY, float velocityZ,
                   HalfType type);
    ~StrawberryHalf() override = default;

    void draw() override;
    void slice(ProjectileManager *manager) override;
    float getRadius() const override;
    void getColor(float &r, float &g, float &b) const override;

    void update(float deltaTime) override;

private:
    HalfType m_type;
    static constexpr float RADIUS = 0.25f;
};

#endif // STRAWBERRYHALF_H
