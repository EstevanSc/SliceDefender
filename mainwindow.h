#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myglwidget.h"
#include "CameraHandler.h"

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

private:
    Ui::MainWindow *ui;
    CameraHandler *cameraHandler;
    int gameScore;

    /**
     * @brief Update the score display
     */
    void updateScoreDisplay();
};

#endif // MAINWINDOW_H
