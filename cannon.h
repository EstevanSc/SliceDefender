#ifndef CANNON_H
#define CANNON_H

#include <GL/glu.h>
#include <QVector3D>
#include <QColor>

class Cannon
{
public:
    Cannon();
    ~Cannon();

    // Méthodes de paramétrage
    void setPosition(const QVector3D &pos);
    void setAngle(float angleDegrees);
    void setLength(float length);
    void setRadius(float radius);
    void setColor(const QColor &color);
    void setSegments(int segments);

    // Paramètres pour les roues
    void setWheelRadius(float radius);
    void setWheelThickness(float thickness);
    void setAxisLength(float length);

    // Accesseurs
    QVector3D getPosition() const;
    float getAngle() const;
    float getLength() const;
    float getRadius() const;
    QColor getColor() const;
    int getSegments() const;
    float getWheelRadius() const;
    float getWheelThickness() const;
    float getAxisLength() const;
    bool isLowPoly() const;

    // Méthode de dessin
    void draw() const;

private:
    // Méthodes auxiliaires de dessin
    void drawCannonTube() const;
    void drawWheels() const;
    void drawReinforcements() const;
    void drawDetails() const;

    // Méthodes utilitaires
    void drawRing(GLUquadric *quadric, float distance, float ringWidth, float scale = 1.05f) const;
    void drawWheel(GLUquadric *quadric, float xOffset) const;

    QVector3D m_position;
    float m_angleDegrees;
    float m_length;
    float m_radius;
    QColor m_color;
    int m_segments;

    // Propriétés pour les roues
    float m_wheelRadius;
    float m_wheelThickness;
    float m_axisLength;
};

#endif // CANNON_H
