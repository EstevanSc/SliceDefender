#ifndef CORRIDOR_H
#define CORRIDOR_H

#include <QOpenGLFunctions>
#include <QVector3D>
#include <QColor>
#include <vector>

/**
 * @class Corridor
 * @brief 3D corridor environment for the game (ground, roof, walls, torches, lighting, procedural tiles/bricks).
 *
 * @author Aubin SIONVILLE
 */
class Corridor : protected QOpenGLFunctions
{
public:
    /**
     * @brief Constructor
     * @param length Length of the corridor (default 25.0)
     * @param width Width of the corridor (default 5.0)
     * @param height Height of the corridor (default 5.0)
     */
    Corridor(float length = 25.0f, float width = 5.0f, float height = 5.0f);

    /**
     * @brief Draws the entire corridor (walls, ground, roof, torches)
     */
    void draw();

private:
    /**
     * @brief Draws the left, right, and end walls with procedural bricks
     */
    void drawWalls();

    /**
     * @brief Draws the ground with procedural stone tiles
     */
    void drawGround();

    /**
     * @brief Draws the roof with procedural bricks
     */
    void drawRoof();

    /**
     * @brief Places and draws torches along the corridor
     */
    void drawTorches();

    /**
     * @brief Draws a single torch and sets up lighting
     * @param position Position of the torch
     * @param leftWall True if torch is on the left wall, false for right wall
     * @param lightIdx Index of the OpenGL light source
     */
    void drawTorch(const QVector3D &position, bool leftWall, int lightIdx);

    /**
     * @brief Draws a single brick
     * @param pos Position of the brick
     * @param w Width
     * @param h Height
     * @param d Depth
     * @param color Color
     */
    void drawBrick(const QVector3D &pos, float w, float h, float d, const QColor &color);

    /**
     * @brief Draws a single stone tile
     * @param pos Position of the tile
     * @param w Width
     * @param d Depth
     * @param color Color
     */
    void drawStoneTile(const QVector3D &pos, float w, float d, const QColor &color);

    /**
     * @brief Draws a flame shape
     * @param pos Position of the flame
     * @param base Base width
     * @param height Height
     */
    void drawFlame(const QVector3D &pos, float base, float height);

    float m_length;
    float m_width;
    float m_height;
    std::vector<std::vector<QColor>> m_tileColors;
};

#endif // CORRIDOR_H
