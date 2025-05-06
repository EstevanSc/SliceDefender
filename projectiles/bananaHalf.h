#ifndef BANANAHALF_H
#define BANANAHALF_H

#include "../projectile.h"
#include <QOpenGLTexture>

class ProjectileManager;

class BananaHalf : public Projectile
{
public:
    enum class HalfType
    {
        FRONT,
        BACK
    };

    BananaHalf(float startX, float startY, float startZ,
               float velocityX, float velocityY, float velocityZ,
               HalfType type);
    ~BananaHalf() override = default;

    void draw() override;
    void slice(ProjectileManager *manager) override;
    float getRadius() const override;
    void getColor(float &r, float &g, float &b) const override;
    void update(float deltaTime) override;

    static constexpr int SEGMENTS = 8;
    static constexpr int SIDES = 10;
    static constexpr float HALF_LENGTH = 0.6f;
    static constexpr float RADIUS = 0.15f;
    static constexpr float CURVE_ANGLE = 55.0f;

private:
    HalfType m_type;
    static QOpenGLTexture *s_bananaTexture; // Static texture for all instances
};

#endif // BANANAHALF_H
