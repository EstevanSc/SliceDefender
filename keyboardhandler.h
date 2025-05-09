#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <QObject>
#include <QVector3D>
#include <QSet>

/**
 * @class KeyboardHandler
 * @brief Tracks keyboard input for player movement
 *
 * This class maintains keyboard state and provides normalized
 * position data similar to the camera handler, allowing for
 * seamless integration with existing player movement code.
 */
class KeyboardHandler : public QObject
{
    Q_OBJECT

public:
    explicit KeyboardHandler(QObject *parent = nullptr);
    ~KeyboardHandler() = default;

    /**
     * @brief Get the current movement direction based on key presses
     * @return Normalized direction vector in the range (-1,-1,0) to (1,1,0)
     */
    QVector3D getMovementDirection() const;

    /**
     * @brief Handle a key press event
     * @param key The key code that was pressed
     */
    void keyPressed(int key);

    /**
     * @brief Handle a key release event
     * @param key The key code that was released
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
     * @brief Signal emitted when the speed multiplier changes
     * @param speedMultiplier The new speed multiplier value
     */
    void speedMultiplierChanged(float speedMultiplier);

private:
    // Set of currently pressed keys
    QSet<int> m_pressedKeys;

    // Current accumulated movement direction
    float m_horizontalMovement;
    float m_verticalMovement;

    // Movement speed
    float m_baseMovementSpeed;
    float m_speedMultiplier;

    // Minimum and maximum speed multiplier values
    static constexpr float MIN_SPEED_MULTIPLIER = 0.5f;
    static constexpr float MAX_SPEED_MULTIPLIER = 4.0f;
    static constexpr float SPEED_CHANGE_STEP = 0.5f;
};

#endif // KEYBOARDHANDLER_H
