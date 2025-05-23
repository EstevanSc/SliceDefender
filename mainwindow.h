#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "myglwidget.h"
#include "CameraHandler.h"
#include "game.h"
#include "scoreboard.h"

namespace Ui
{
    class MainWindow;
}

/**
 * @class MainWindow
 * @brief Main application window for Slice Defender (game, camera, UI overlays).
 *
 * Manages the main window, OpenGL game view, camera interface, score/statistics, overlays, and user interactions.
 *
 * @author Estevan SCHMITT, Aubin SIONVILLE
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor. Initializes UI and camera resources.
     * @param parent Parent widget
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor. Cleans up resources.
     */
    ~MainWindow();

    /**
     * @brief Returns the current game mode (standard/original)
     * @return True if standard mode, false if original mode
     */
    bool isStandardMode() const { return m_standardMode; }

private slots:
    /**
     * @brief Starts a new game and resets the score
     */
    void startNewGame();

    /**
     * @brief Displays the about dialog
     */
    void showAboutDialog();

    /**
     * @brief Starts or restarts the game (resets state and begins countdown)
     */
    void startGame();

    /**
     * @brief Updates the score display
     * @param score New score value
     */
    void updateScoreLabel(int score);

    /**
     * @brief Updates the lives display
     * @param lives Remaining lives
     */
    void updateLivesLabel(int lives);

    /**
     * @brief Updates the countdown display
     * @param value Current countdown value
     */
    void updateCountdownLabel(int value);

    /**
     * @brief Shows the start button when the game ends
     */
    void showStartButton();

    /**
     * @brief Updates the speed indicator when the keyboard speed changes
     * @param speedMultiplier The new speed multiplier value
     */
    void updateSpeedIndicator(float speedMultiplier);

    /**
     * @brief Toggles the scoreboard overlay visibility
     */
    void toggleScoreboard();

    /**
     * @brief Saves the current score with the player's name
     */
    void savePlayerScore();

    /**
     * @brief Toggles the instructions overlay visibility
     */
    void toggleInstructions();

    /**
     * @brief Toggles between standard and original game modes
     *
     * Standard mode: 1 life, 1 point per projectile
     * Original mode: 5 lives, combo scoring system
     */
    void toggleGameMode();

    /**
     * @brief Toggles between internal (0) and external (1) camera sources
     *
     * This slot is connected to the cameraSwitchButton in Qt Designer.
     * It switches between camera indices 0 and 1, and updates the button text
     * to reflect which camera is currently active.
     */
    void toggleCameraSource();

private:
    Ui::MainWindow *ui; // Pointer to the UI components
    CameraHandler *cameraHandler; // Pointer to the camera handler
    Game *game; // Pointer to the game logic

    int gameScore; // Current score

    bool m_showingScoreboard; // Flag to indicate if the scoreboard is visible
    Scoreboard *m_scoreboard; // Pointer to the scoreboard object

    QTimer *m_statusTimer; // Timer for updating status messages
    QTimer *m_gameTimer; // Timer for triggering display updates (10ms intervals)
    QTime m_gameStartTime; // Stores the exact time when the game started

    bool m_showingInstructions; // Flag to indicate if the instructions overlay is visible
    bool m_standardMode = true; // Flag to indicate if the game is in standard mode (true) or original mode (false)
    int cameraIndex_ = 0; // index of the currently active camera (0 = internal, 1 = external)
    /**
     * @brief Updates the score display
     */
    void updateScoreDisplay();

    /**
     * @brief Updates the scoreboard overlay with the latest scores
     */
    void updateScoreboardDisplay();

    /**
     * @brief Displays a status message in the scoreboard overlay
     * @param message The message to display
     * @param success Whether the operation was successful (green) or failed (red)
     */
    void showStatusMessage(const QString &message, bool success);

    /**
     * @brief Updates the game timer display with centisecond precision
     *
     * Calculates and displays the elapsed time since game start in
     * mm:ss:XX format (minutes:seconds:centiseconds)
     */
    void updateGameTimeDisplay();
};

#endif // MAINWINDOW_H
