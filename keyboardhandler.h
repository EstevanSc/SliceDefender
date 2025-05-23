#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <QObject>
#include <QVector3D>
#include <QSet>

/**
 * @class KeyboardHandler
 * @brief Handles keyboard input for player movement and speed control.
 *
 * @author Estevan SCHMITT
 */
class KeyboardHandler : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent QObject (default nullptr)
     */
    explicit KeyboardHandler(QObject *parent = nullptr);
    ~KeyboardHandler() = default;

    /**
     * @brief Get the current movement direction based on key presses
     * @return Direction vector (X, Y, 0), scaled by speed and factor
     */
    QVector3D getMovementDirection() const;

    /**
     * @brief Handle a key press event
     * @param key The key code pressed
     */
    void keyPressed(int key);

    /**
     * @brief Handle a key release event
     * @param key The key code released
     */
    void keyReleased(int key);

    /**
     * @brief Check if any movement keys are currently pressed
     * @return True if any movement keys are pressed
     */
    bool isMoving() const;

    /**
     * @brief Get the current speed multiplier
     * @return The speed multiplier value
     */
    float getSpeedMultiplier() const { return m_speedMultiplier; }

signals:
    /**
     * @brief Emitted when the speed multiplier changes
     * @param speedMultiplier The new speed multiplier value
     */
    void speedMultiplierChanged(float speedMultiplier);

private:
    QSet<int> m_pressedKeys; // Set of currently pressed keys
    float m_horizontalMovement; // Horizontal movement speed
    float m_verticalMovement; // Vertical movement speed
    float m_baseMovementSpeed; // Base movement speed
    float m_speedMultiplier; // Speed multiplier for movement

    // Minimum and maximum speed multiplier values
    static constexpr float MIN_SPEED_MULTIPLIER = 0.5f; // Minimum speed multiplier
    static constexpr float MAX_SPEED_MULTIPLIER = 4.0f; // Maximum speed multiplier
    static constexpr float SPEED_CHANGE_STEP = 0.5f; // Speed change step
    static constexpr float SWORD_SPEED_FACTOR = 0.5f; // Sword speed factor
};

#endif // KEYBOARDHANDLER_H
