#ifndef CORRIDOR_H
#define CORRIDOR_H

#include <QOpenGLFunctions>
#include <QVector3D>
#include <QColor>
#include <vector>

class Corridor : protected QOpenGLFunctions
{
public:
    Corridor(float length = 25.0f, float width = 5.0f, float height = 5.0f);
    void draw();

private:
    void drawWalls();
    void drawGround();
    void drawRoof();
    void drawTorches();
    void drawTorch(const QVector3D &position, bool leftWall, int lightIdx);
    void drawBrick(const QVector3D &pos, float w, float h, float d, const QColor &color);
    void drawStoneTile(const QVector3D &pos, float w, float d, const QColor &color);
    void drawFlame(const QVector3D &pos, float base, float height);
    float getProceduralVariation(int x, int y, int z, float amplitude) const;

    float m_length;
    float m_width;
    float m_height;

    // Couleurs fixes pour les pavés du sol
    std::vector<std::vector<QColor>> m_tileColors;
};

#endif // CORRIDOR_H
