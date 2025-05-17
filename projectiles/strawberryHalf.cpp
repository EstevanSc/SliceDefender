#include <GL/gl.h>
#include <GL/glu.h>
#include "strawberryHalf.h"
#include "../projectileManager.h"
#include <QOpenGLTexture>
#include <QImage>

static QOpenGLTexture *g_strawberryTexture = nullptr;

StrawberryHalf::StrawberryHalf(float startX, float startY, float startZ,
                               float velocityX, float velocityY, float velocityZ,
                               HalfType type)
    : Projectile(startX, startY, startZ, velocityX, velocityY, velocityZ),
      m_type(type)
{
    if (!g_strawberryTexture)
        g_strawberryTexture = new QOpenGLTexture(QImage(":/strawberry_color.jpg").mirrored());
}

void StrawberryHalf::draw()
{
    if (!isActive())
        return;

    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);
    glScalef(0.7f, 1.0f, 0.7f);

    // --- Corps de la fraise (rouge, texturé) ---
    static GLuint strawberryTex = 0;
    if (strawberryTex == 0)
    {
        QImage img(":/strawberry_color.jpg");
        if (!img.isNull())
        {
            img = img.convertToFormat(QImage::Format_RGBA8888);
            glGenTextures(1, &strawberryTex);
            glBindTexture(GL_TEXTURE_2D, strawberryTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        }
    }

    glColor3f(1.0f, 1.0f, 1.0f); // White for texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, strawberryTex);

    const int sides = 4;
    const int stacks = 4;
    const float height = 0.8f;
    const float baseRadius = 0.7f;

    // Texture zoom factor (>1 means zoom in)
    const float texZoom = 2.5f;

    // Clip for half strawberry
    double planeEq[4] = {0.0, 0.0, 0.0, 0.0};
    planeEq[0] = (m_type == HalfType::LEFT) ? 1.0 : -1.0;
    glClipPlane(GL_CLIP_PLANE0, planeEq);
    glEnable(GL_CLIP_PLANE0);

    for (int j = 0; j < stacks; ++j)
    {
        float t0 = float(j) / stacks;
        float t1 = float(j + 1) / stacks;
        float y0 = height * (1.0f - t0);
        float y1 = height * (1.0f - t1);

        // Reduce the radius more at the bottom for a pointier strawberry
        float r0 = baseRadius * (1.0f - t0 * 0.8f) * (0.6f + 0.4f * std::sin(M_PI * t0));
        float r1 = baseRadius * (1.0f - t1 * 0.8f) * (0.6f + 0.4f * std::sin(M_PI * t1));

        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= sides; ++i)
        {
            float angle = 2.0f * M_PI * i / sides;
            float x0 = r0 * std::cos(angle);
            float z0 = r0 * std::sin(angle);
            float x1 = r1 * std::cos(angle);
            float z1 = r1 * std::sin(angle);

            // Texture coordinates, zoomed in
            float u = (std::cos(angle) * 0.5f + 0.5f) / texZoom + 0.5f - 0.5f / texZoom;
            float v0 = (1.0f - t0) / texZoom + 0.5f - 0.5f / texZoom;
            float v1 = (1.0f - t1) / texZoom + 0.5f - 0.5f / texZoom;

            glNormal3f(x0, 0.3f, z0);
            glTexCoord2f(u, v0);
            glVertex3f(x0, y0, z0);

            glNormal3f(x1, 0.3f, z1);
            glTexCoord2f(u, v1);
            glVertex3f(x1, y1, z1);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    // Draw a round green circle (calyx) at the top of the strawberry
    float r = baseRadius * 0.3f; // r is the top radius of the strawberry
    int circleSegments = 24;
    glColor3f(0.1f, 0.8f, 0.1f); // Green
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, height, 0.0f); // Center of the circle
    for (int i = 0; i <= circleSegments; ++i)
    {
        float angle = 2.0f * M_PI * i / circleSegments;
        float x = r * std::cos(angle);
        float z = r * std::sin(angle);
        glVertex3f(x, height, z);
    }
    glEnd();

    // Green leaves
    glColor3f(0.1f, 0.8f, 0.1f); // Green

    const int leafNumber = 6; // You can adjust the number of leaves
    const int leafSegments = 4;
    const float leafLength = 0.4f;
    const float leafWidth = 0.15f;
    const float leafCurve = 0.1f; // Controls how much the leaf curves upward

    float leafStart = 0.9f * r; // Start the leaf at radius 0.9r

    for (int leaf = 0; leaf < leafNumber; ++leaf)
    {
        float angle = 2.0f * M_PI * leaf / leafNumber;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        glBegin(GL_QUADS);
        for (int i = 0; i < leafSegments; ++i)
        {
            float t0 = float(i) / leafSegments;
            float t1 = float(i + 1) / leafSegments;

            // Position along the leaf (from start outward)
            float x0 = leafStart + t0 * leafLength;
            float x1 = leafStart + t1 * leafLength;

            // Curve upward: y increases with t^2
            float y0 = height + leafCurve * t0 * t0;
            float y1 = height + leafCurve * t1 * t1;

            // Two vertices per segment (top/bottom of the leaf width)
            // Rotate and translate each vertex
            float vx0a = x0 * cosA;
            float vz0a = x0 * sinA;
            float vx1a = x1 * cosA;
            float vz1a = x1 * sinA;

            // Offset leaf width perpendicular to leaf direction
            float wx = -sinA * (leafWidth / 2);
            float wz = cosA * (leafWidth / 2);

            glVertex3f(vx0a + wx, y0, vz0a + wz);
            glVertex3f(vx0a - wx, y0, vz0a - wz);
            glVertex3f(vx1a - wx, y1, vz1a - wz);
            glVertex3f(vx1a + wx, y1, vz1a + wz);
        }
        glEnd();
    }

    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
}

void StrawberryHalf::update(float deltaTime)
{
    m_acceleration[0] = 0.0f;
    m_acceleration[1] = -GRAVITY;
    m_acceleration[2] = 0.0f;

    m_velocity[0] += m_acceleration[0] * deltaTime;
    m_velocity[1] += m_acceleration[1] * deltaTime;
    m_velocity[2] += m_acceleration[2] * deltaTime;

    m_position[0] += m_velocity[0] * deltaTime;
    m_position[1] += m_velocity[1] * deltaTime;
    m_position[2] += m_velocity[2] * deltaTime;

    if (m_position[1] <= 0.0f || m_position[2] >= 0.0f || m_position[2] <= -30.0f)
    {
        m_isActive = false;
    }
}

void StrawberryHalf::slice(ProjectileManager *manager)
{
    // Halves cannot be sliced
}

float StrawberryHalf::getRadius() const
{
    return RADIUS;
}

void StrawberryHalf::getColor(float &r, float &g, float &b) const
{
    r = 1.0f;
    g = 0.0f;
    b = 0.0f; // Red for strawberry
}
