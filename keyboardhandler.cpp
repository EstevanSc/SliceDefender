#include "keyboardhandler.h"
#include <Qt>
#include <QDebug>

KeyboardHandler::KeyboardHandler(QObject *parent)
    : QObject(parent),
      m_horizontalMovement(0.0f),
      m_verticalMovement(0.0f),
      m_baseMovementSpeed(0.1f),
      m_speedMultiplier(1.0f)
{
}

QVector3D KeyboardHandler::getMovementDirection() const
{
    // Returns the current movement direction vector, scaled by speed and factor
    return QVector3D(m_horizontalMovement * m_speedMultiplier * SWORD_SPEED_FACTOR,
                     m_verticalMovement * m_speedMultiplier * SWORD_SPEED_FACTOR,
                     0.0f);
}

void KeyboardHandler::keyPressed(int key)
{
    // Register key press and update movement or speed accordingly
    if (!m_pressedKeys.contains(key))
    {
        m_pressedKeys.insert(key);
        switch (key)
        {
        case Qt::Key_Left:
        case Qt::Key_Q:
        case Qt::Key_A:
            m_horizontalMovement = -m_baseMovementSpeed;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            m_horizontalMovement = m_baseMovementSpeed;
            break;
        case Qt::Key_Up:
        case Qt::Key_Z:
        case Qt::Key_W:
            m_verticalMovement = m_baseMovementSpeed;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            m_verticalMovement = -m_baseMovementSpeed;
            break;
        case Qt::Key_Plus:
        case Qt::Key_Asterisk:
            m_speedMultiplier = qMin(m_speedMultiplier + SPEED_CHANGE_STEP, MAX_SPEED_MULTIPLIER);
            qDebug() << "Speed increased to:" << m_speedMultiplier;
            emit speedMultiplierChanged(m_speedMultiplier);
            break;
        case Qt::Key_Minus:
            m_speedMultiplier = qMax(m_speedMultiplier - SPEED_CHANGE_STEP, MIN_SPEED_MULTIPLIER);
            qDebug() << "Speed decreased to:" << m_speedMultiplier;
            emit speedMultiplierChanged(m_speedMultiplier);
            break;
        }
    }
}

void KeyboardHandler::keyReleased(int key)
{
    // Unregister key and reset movement if needed
    m_pressedKeys.remove(key);
    switch (key)
    {
    case Qt::Key_Left:
    case Qt::Key_Q:
    case Qt::Key_A:
        if (!m_pressedKeys.contains(Qt::Key_Right) &&
            !m_pressedKeys.contains(Qt::Key_D))
        {
            m_horizontalMovement = 0.0f;
        }
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        if (!m_pressedKeys.contains(Qt::Key_Left) &&
            !m_pressedKeys.contains(Qt::Key_Q) &&
            !m_pressedKeys.contains(Qt::Key_A))
        {
            m_horizontalMovement = 0.0f;
        }
        break;
    case Qt::Key_Up:
    case Qt::Key_Z:
    case Qt::Key_W:
        if (!m_pressedKeys.contains(Qt::Key_Down) &&
            !m_pressedKeys.contains(Qt::Key_S))
        {
            m_verticalMovement = 0.0f;
        }
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        if (!m_pressedKeys.contains(Qt::Key_Up) &&
            !m_pressedKeys.contains(Qt::Key_Z) &&
            !m_pressedKeys.contains(Qt::Key_W))
        {
            m_verticalMovement = 0.0f;
        }
        break;
    }
}

bool KeyboardHandler::isMoving() const
{
    // Returns true if any movement is currently active
    return m_horizontalMovement != 0.0f || m_verticalMovement != 0.0f;
}
