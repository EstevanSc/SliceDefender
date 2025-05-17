#include <GL/gl.h>
#include <GL/glu.h>
#include "banana.h"
#include "../projectileManager.h"
#include "bananaHalf.h"
#include <QOpenGLTexture>
#include <QImage>
#include <vector>
#include <QVector3D>
#include <QVector2D>
#include <random>

static QOpenGLTexture *g_bananaTexture = nullptr;

Banana::Banana(float startX, float startY, float startZ, float velocityX, float velocityY, float velocityZ)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ)
{
    if (!g_bananaTexture)
        g_bananaTexture = new QOpenGLTexture(QImage(":/banana_color.jpg").mirrored());
}

void Banana::draw()
{
    if (!isActive())
        return;

    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);

    float angle = m_rotationSpeed * m_rotationTime;
    glRotatef(angle, m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);

    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(-m_position[0], -m_position[1], -m_position[2]);

    // Parameters
    const int segments = SEGMENTS;
    const int sides = SIDES;
    const float length = LENGTH;
    const float radius = RADIUS;
    const float curveAngle = CURVE_ANGLE;

    // Curvature around Z (arc in the XY plane, Z constant)
    QVector3D center(m_position[0], m_position[1], m_position[2]);
    float startAngle = -curveAngle / 2.0f;

    std::vector<std::vector<QVector3D>> ringVertices(segments + 1);
    std::vector<std::vector<QVector2D>> ringTexCoords(segments + 1);

    for (int i = 0; i <= segments; ++i) {
        float t = float(i) / segments;
        float angle = startAngle + t * curveAngle;
        float rad = angle * M_PI / 180.0f;
        float arcRadius = length / (curveAngle * M_PI / 180.0f);

        // Curvature in the XY plane, Z constant
        float cx = center.x() + arcRadius * std::cos(rad);
        float cy = center.y() + arcRadius * std::sin(rad);
        float cz = center.z();

        float tangentAngle = rad + M_PI_2;
        float scale = 0.5f + 0.5f * std::sin(M_PI * t);

        for (int j = 0; j < sides; ++j) {
            float theta = 2.0f * M_PI * j / sides;
            float x = std::cos(theta) * radius * scale;
            float y = std::sin(theta) * radius * scale;
            
            // Section perpendicular to the arc (in the Z plane)
            float px = cx - y * std::sin(tangentAngle);
            float py = cy + y * std::cos(tangentAngle);
            float pz = cz + x;
            ringVertices[i].push_back(QVector3D(px, py, pz));
            ringTexCoords[i].push_back(QVector2D(float(j) / sides, t));
        }
    }

    // Textured yellow part
    if (g_bananaTexture) {
        glEnable(GL_TEXTURE_2D);
        g_bananaTexture->bind();
    }
    glColor3f(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < sides; ++j) {
            int nextJ = (j + 1) % sides;
            QVector3D v0 = ringVertices[i][j];
            QVector3D v1 = ringVertices[i][nextJ];
            QVector3D v2 = ringVertices[i + 1][nextJ];
            QVector3D v3 = ringVertices[i + 1][j];
            QVector2D t0 = ringTexCoords[i][j];
            QVector2D t1 = ringTexCoords[i][nextJ];
            QVector2D t2 = ringTexCoords[i + 1][nextJ];
            QVector2D t3 = ringTexCoords[i + 1][j];
            QVector3D normal = QVector3D::normal(v0, v1, v2);

            glBegin(GL_QUADS);
            glNormal3f(normal.x(), normal.y(), normal.z());
            glTexCoord2f(t0.x(), t0.y()); glVertex3f(v0.x(), v0.y(), v0.z());
            glTexCoord2f(t1.x(), t1.y()); glVertex3f(v1.x(), v1.y(), v1.z());
            glTexCoord2f(t2.x(), t2.y()); glVertex3f(v2.x(), v2.y(), v2.z());
            glTexCoord2f(t3.x(), t3.y()); glVertex3f(v3.x(), v3.y(), v3.z());
            glEnd();
        }
    }

    if (g_bananaTexture) {
        g_bananaTexture->release();
        glDisable(GL_TEXTURE_2D);
    }

    // Brown ends (caps)
    const float capExtrudeStart = 0.08f;
    const float capScaleStart = 0.3f;
    const float capExtrudeEnd = 0.15f;
    const float capScaleEnd = 1.2f;

    // Start cap (small)
    glColor3f(0.4f, 0.2f, 0.05f);
    QVector3D coreStart = ringVertices[0][0];
    QVector3D coreNext = ringVertices[1][0];
    QVector3D capDirStart = (coreStart - coreNext).normalized();
    std::vector<QVector3D> capStartExtruded(sides);
    for (int j = 0; j < sides; ++j) {
        QVector3D fromCenter = ringVertices[0][j] - coreStart;
        capStartExtruded[j] = coreStart + fromCenter * capScaleStart + capDirStart * capExtrudeStart;
    }
    for (int j = 0; j < sides; ++j) {
        int nextJ = (j + 1) % sides;
        QVector3D v0 = ringVertices[0][j];
        QVector3D v1 = ringVertices[0][nextJ];
        QVector3D v2 = capStartExtruded[nextJ];
        QVector3D v3 = capStartExtruded[j];
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
    QVector3D capNormalStart = capDirStart;
    glNormal3f(capNormalStart.x(), capNormalStart.y(), capNormalStart.z());
    for (int j = 0; j < sides; ++j) {
        QVector3D v = capStartExtruded[j];
        glVertex3f(v.x(), v.y(), v.z());
    }
    glEnd();

    // End cap (large)
    glColor3f(0.4f, 0.2f, 0.05f);
    QVector3D coreEnd = ringVertices[segments][0];
    QVector3D corePrev = ringVertices[segments - 1][0];
    QVector3D capDirEnd = (coreEnd - corePrev).normalized();
    std::vector<QVector3D> capEndExtruded(sides);
    for (int j = 0; j < sides; ++j) {
        QVector3D fromCenter = ringVertices[segments][j] - coreEnd;
        capEndExtruded[j] = coreEnd + fromCenter * capScaleEnd + capDirEnd * capExtrudeEnd;
    }
    for (int j = 0; j < sides; ++j) {
        int nextJ = (j + 1) % sides;
        QVector3D v0 = ringVertices[segments][j];
        QVector3D v1 = ringVertices[segments][nextJ];
        QVector3D v2 = capEndExtruded[nextJ];
        QVector3D v3 = capEndExtruded[j];
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
    QVector3D capNormalEnd = capDirEnd;
    glNormal3f(capNormalEnd.x(), capNormalEnd.y(), capNormalEnd.z());
    for (int j = sides - 1; j >= 0; --j) {
        QVector3D v = capEndExtruded[j];
        glVertex3f(v.x(), v.y(), v.z());
    }
    glEnd();

    glPopMatrix();
}

void Banana::slice(ProjectileManager *manager)
{
    // Slice into two halves (left/right, force on X)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dxDist(0.5f, 1.5f);

    float dx = dxDist(gen);

    // Left : impulse towards -X
    BananaHalf *frontHalf = new BananaHalf(
        m_position[0] - dx * 0.2f, m_position[1], m_position[2],
        m_velocity[0] - dx, m_velocity[1], m_velocity[2],
        BananaHalf::HalfType::FRONT);

    // Right : impulse towards +X
    BananaHalf *backHalf = new BananaHalf(
        m_position[0] + dx * 0.2f, m_position[1], m_position[2],
        m_velocity[0] + dx, m_velocity[1], m_velocity[2],
        BananaHalf::HalfType::BACK);


    frontHalf->setRotationAxis(m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    frontHalf->setRotationSpeed(m_rotationSpeed);
    frontHalf->setRotationTime(m_rotationTime);
    backHalf->setRotationAxis(m_rotationAxis[0], m_rotationAxis[1], m_rotationAxis[2]);
    backHalf->setRotationSpeed(m_rotationSpeed);
    backHalf->setRotationTime(m_rotationTime);

    manager->addProjectile(frontHalf);
    manager->addProjectile(backHalf);
}

float Banana::getRadius() const
{
    return RADIUS;
}

void Banana::getColor(float &r, float &g, float &b) const
{
    r = 1.0f;
    g = 1.0f;
    b = 0.0f;
}
