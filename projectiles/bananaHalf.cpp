#include <GL/gl.h>
#include <GL/glu.h>
#include "bananaHalf.h"
#include "../projectileManager.h"
#include <QOpenGLTexture>
#include <QImage>
#include <vector>
#include <QVector3D>
#include <QVector2D>
#include <random>

QOpenGLTexture *BananaHalf::s_bananaTexture = nullptr;

BananaHalf::BananaHalf(float startX, float startY, float startZ,
                       float velocityX, float velocityY, float velocityZ,
                       HalfType type)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ),
      m_type(type)
{
    if (!s_bananaTexture)
        s_bananaTexture = new QOpenGLTexture(QImage(":/banana_color.jpg").mirrored());
}

void BananaHalf::draw()
{
    if (!isActive())
        return;

    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    // Apply the rotation of the banana half (inherited from the original banana)
    float angle = m_rotationSpeed * m_rotationTime;
    glRotatef(angle, m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(-m_position[0], -m_position[1], -m_position[2]);

    // Parameters
    const int segments = SEGMENTS;
    const int sides = SIDES;
    const float length = HALF_LENGTH;
    const float radius = RADIUS;
    const float curveAngle = CURVE_ANGLE;

    // Arc in XY plane, Z constant
    QVector3D center(m_position[0], m_position[1], m_position[2]);
    float startAngle = (m_type == HalfType::FRONT) ? -curveAngle : 0.0f;

    std::vector<std::vector<QVector3D>> ringVertices(segments + 1);
    std::vector<std::vector<QVector2D>> ringTexCoords(segments + 1);

    for (int i = 0; i <= segments; ++i)
    {
        float t = float(i) / segments;
        float angle = startAngle + t * curveAngle;
        float rad = angle * M_PI / 180.0f;
        float arcRadius = length / (curveAngle * M_PI / 180.0f);

        float cx = center.x() + arcRadius * std::cos(rad);
        float cy = center.y() + arcRadius * std::sin(rad);
        float cz = center.z();

        float tangentAngle = rad + M_PI_2;

        // Reproduce the thickness variation of the whole banana on the corresponding half
        float tGlobal;
        if (m_type == HalfType::FRONT) {
            tGlobal = t * 0.5f; // LEFT = 0 to 0.5 of the whole banana
        } else {
            tGlobal = 0.5f + t * 0.5f; // RIGHT = 0.5 to 1 of the whole banana
        }
        float scale = 0.5f + 0.5f * std::sin(M_PI * tGlobal);

        for (int j = 0; j < sides; ++j)
        {
            float theta = 2.0f * M_PI * j / sides;
            float x = std::cos(theta) * radius * scale;
            float y = std::sin(theta) * radius * scale;
            float px = cx - y * std::sin(tangentAngle);
            float py = cy + y * std::cos(tangentAngle);
            float pz = cz + x;
            ringVertices[i].push_back(QVector3D(px, py, pz));
            ringTexCoords[i].push_back(QVector2D(float(j) / sides, t));
        }
    }

    // Yellow textured part
    if (s_bananaTexture)
    {
        glEnable(GL_TEXTURE_2D);
        s_bananaTexture->bind();
    }
    glColor3f(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < segments; ++i)
    {
        for (int j = 0; j < sides; ++j)
        {
            int nextJ = (j + 1) % sides;
            QVector3D v0 = ringVertices[i][j];
            QVector3D v1 = ringVertices[i][nextJ];
            QVector3D v2 = ringVertices[i + 1][nextJ];
            QVector3D v3 = ringVertices[i + 1][j];
            QVector2D t0 = ringTexCoords[i][j];
            QVector2D t1 = ringTexCoords[i][nextJ];
            QVector2D t2 = ringTexCoords[i + 1][nextJ];
            QVector2D t3 = ringTexCoords[i + 1][j];

            // Calculate the normal from the triangular face
            // Use v0, v1, v2 to compute a normal that points outward
            QVector3D edge1 = v1 - v0;
            QVector3D edge2 = v2 - v0;
            QVector3D normal = QVector3D::crossProduct(edge1, edge2).normalized();

            glBegin(GL_QUADS);
            glNormal3f(normal.x(), normal.y(), normal.z());
            glTexCoord2f(t0.x(), t0.y());
            glVertex3f(v0.x(), v0.y(), v0.z());
            glTexCoord2f(t1.x(), t1.y());
            glVertex3f(v1.x(), v1.y(), v1.z());
            glTexCoord2f(t2.x(), t2.y());
            glVertex3f(v2.x(), v2.y(), v2.z());
            glTexCoord2f(t3.x(), t3.y());
            glVertex3f(v3.x(), v3.y(), v3.z());
            glEnd();
        }
    }

    if (s_bananaTexture)
    {
        s_bananaTexture->release();
        glDisable(GL_TEXTURE_2D);
    }

    // Brown caps (ends)
    const float capExtrude = (m_type == HalfType::FRONT) ? 0.08f : 0.15f;
    const float capScale = (m_type == HalfType::FRONT) ? 0.3f : 1.2f;
    glColor3f(0.4f, 0.2f, 0.05f);

    int capIndex = (m_type == HalfType::FRONT) ? 0 : segments;
    int capNext = (m_type == HalfType::FRONT) ? 1 : segments - 1;
    QVector3D core = ringVertices[capIndex][0];
    QVector3D coreNext = ringVertices[capNext][0];
    QVector3D capDir = (core - coreNext).normalized();
    std::vector<QVector3D> capExtruded(sides);
    for (int j = 0; j < sides; ++j)
    {
        QVector3D fromCenter = ringVertices[capIndex][j] - core;
        capExtruded[j] = core + fromCenter * capScale + capDir * capExtrude;
    }
    for (int j = 0; j < sides; ++j)
    {
        int nextJ = (j + 1) % sides;
        QVector3D v0 = ringVertices[capIndex][j];
        QVector3D v1 = ringVertices[capIndex][nextJ];
        QVector3D v2 = capExtruded[nextJ];
        QVector3D v3 = capExtruded[j];
        QVector3D normal = QVector3D::normal(v0, v1, v2);
        glBegin(GL_QUADS);
        glNormal3f(normal.x(), normal.y(), normal.z());
        glVertex3f(v0.x(), v0.y(), v0.z());
        glVertex3f(v1.x(), v1.y(), v1.z());
        glVertex3f(v2.x(), v2.y(), v2.z());
        glVertex3f(v3.x(), v3.y(), v3.z());
        glEnd();
    }
    glBegin(GL_POLYGON);
    QVector3D capNormal = capDir;
    glNormal3f(capNormal.x(), capNormal.y(), capNormal.z());
    if (m_type == HalfType::FRONT)
    {
        for (int j = 0; j < sides; ++j)
        {
            QVector3D v = capExtruded[j];
            glVertex3f(v.x(), v.y(), v.z());
        }
    }
    else
    {
        for (int j = sides - 1; j >= 0; --j)
        {
            QVector3D v = capExtruded[j];
            glVertex3f(v.x(), v.y(), v.z());
        }
    }
    glEnd();

    glPopMatrix();
}

void BananaHalf::update(float deltaTime)
{
    // Update physics
    m_acceleration[0] = 0.0f;
    m_acceleration[1] = -GRAVITY;
    m_acceleration[2] = 0.0f;

    // Update velocity based on acceleration
    m_velocity[0] += m_acceleration[0] * deltaTime;
    m_velocity[1] += m_acceleration[1] * deltaTime;
    m_velocity[2] += m_acceleration[2] * deltaTime;

    // Update position based on velocity
    m_position[0] += m_velocity[0] * deltaTime;
    m_position[1] += m_velocity[1] * deltaTime;
    m_position[2] += m_velocity[2] * deltaTime;

    // Deactivate the projectile if it goes out of bounds
    if (m_position[1] <= 0.0f || m_position[2] >= 0.0f || m_position[2] <= -30.0f)
    {
        m_isActive = false;
    }
}

void BananaHalf::slice(ProjectileManager *manager)
{
    // Halves cannot be sliced
    // Do nothing
}

float BananaHalf::getRadius() const
{
    return RADIUS;
}
