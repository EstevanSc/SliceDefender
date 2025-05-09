#ifndef CORN_HALF_H
#define CORN_HALF_H

#include "../projectile.h"

class CornHalf : public Projectile
{
public:
    enum class HalfType
    {
        FRONT,
        BACK
    };

    CornHalf(float startX, float startY, float startZ,
             float velocityX, float velocityY, float velocityZ,
             HalfType type);
    ~CornHalf() override = default;

    void draw() override;
    void slice(ProjectileManager *manager) override;
    float getRadius() const override;
    void getColor(float &r, float &g, float &b) const override;
    void update(float deltaTime) override;

private:
    HalfType m_type;
    static constexpr float RADIUS = 0.18f;
    static constexpr float LENGTH = 0.35f; // Half length of the whole corn
};

#endif // CORN_HALF_H
