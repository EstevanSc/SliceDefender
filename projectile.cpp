#include "projectile.h"
#include <cmath>
#include <iostream>
#include <QTime>

Projectile::Projectile(int finalPosition[3], int startPosition[3], float flyingTime)
{
    // set the final position of the projectile
    m_finalPosition[0] = finalPosition[0];
    m_finalPosition[1] = finalPosition[1];
    m_finalPosition[2] = finalPosition[2];
    // set the start position of the projectile
    m_startPosition[0] = startPosition[0];
    m_startPosition[1] = startPosition[1];
    m_startPosition[2] = startPosition[2];
    // set the flying time of the projectile
    m_flyingTime = flyingTime;
    // put the time of launch to current time
    m_timeOfLaunch = QTime::currentTime().msecsSinceStartOfDay() / 1000.0f;
}


void Projectile::move()
{
    // Calcul of the angle of the canon in radians
    double alpha = canonAngle * 3.14 / 180.0;

    // Calcul of the distance between the start and final position
    double dx = m_finalPosition[0] - m_startPosition[0];
    double dy = m_finalPosition[1] - m_startPosition[1];
    double dz = m_finalPosition[2] - m_startPosition[2];
    double g = 9.81;

    double Dxy = std::sqrt(dx * dx + dy * dy);

    // Calcul of the speed of the projectile
    double v0 = Dxy / (m_flyingTime * std::cos(alpha));

    // t is the time since the launch
    double t = QTime::currentTime().msecsSinceStartOfDay() / 1000.0f - m_timeOfLaunch;

    // Calcul of the new position of the projectile
    double x = m_startPosition[0] + (dx / m_flyingTime) * t;
    double y = m_startPosition[1] + (dy / m_flyingTime) * t;
    double z = m_startPosition[2] + v0 * std::sin(alpha) * t - 0.5 * g * t * t;

    this->setPosition(x, y, z);
}
