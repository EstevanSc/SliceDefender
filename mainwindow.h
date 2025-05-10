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
 * @brief The main application window containing the game and camera interface
 *
 * This class manages the main window of the Slice Defender game, including:
 * - OpenGL rendering area for the main game
 * - Camera processing via CameraHandler in the top-right corner
 * - Score and game statistics in the bottom-right corner
 * - Scoreboard overlay for displaying high scores
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor initializes UI and camera resources
     * @param parent Parent widget
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor cleans up resources
     */
    ~MainWindow();

private slots:
    /**
     * @brief Start a new game with reset score
     */
    void startNewGame();

    /**
     * @brief Display the about dialog
     */
    void showAboutDialog();

    /**
     * @brief Update the match quality display
     * @param quality Match quality value
     */
    void updateMatchQuality(int quality);

    /**
     * @brief Starts or restarts the game
     * Resets the game state and begins countdown
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
     * @brief Shows the start button when game ends
     */
    void showStartButton();

    /**
     * @brief Updates the speed indicator when the keyboard speed changes
     * @param speedMultiplier The new speed multiplier value
     */
    void updateSpeedIndicator(float speedMultiplier);

    /**
     * @brief Toggles the scoreboard visibility
     */
    void toggleScoreboard();

    /**
     * @brief Saves the current score with the player's name
     */
    void savePlayerScore();

private:
    Ui::MainWindow *ui;
    CameraHandler *cameraHandler;
    Game *game;
    int gameScore;
    bool m_showingScoreboard;
    Scoreboard *m_scoreboard;
    QTimer *m_statusTimer;

    /**
     * @brief Update the score display
     */
    void updateScoreDisplay();

    /**
     * @brief Updates the scoreboard display with the latest scores
     */
    void updateScoreboardDisplay();

    /**
     * @brief Displays a status message in the scoreboard overlay
     * @param message The message to display
     * @param success Whether the operation was successful (green) or failed (red)
     */
    void showStatusMessage(const QString &message, bool success);
};

#endif // MAINWINDOW_H
