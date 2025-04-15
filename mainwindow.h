#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include "myglwidget.h"

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
 * - Camera processing in the top-right corner
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
     * @brief Update the camera frame and process it
     */
    void updateCameraFrame();

    /**
     * @brief Handle the capture button click
     */
    void on_captureButton_clicked();

private:
    Ui::MainWindow *ui;

    // Camera handling
    cv::VideoCapture *camera;
    QTimer *cameraTimer;
    bool isCameraActive;
    bool isDetectionActive;

    // Game score
    int gameScore;

    // Feature detection
    int featureCount;
    cv::Rect detectionArea;

    /**
     * @brief Initialize camera resources
     * @return True if camera was successfully opened
     */
    bool initializeCamera();

    /**
     * @brief Process image with feature detection
     * @param frame Current camera frame
     * @return Number of features detected
     */
    int processFrame(cv::Mat &frame);

    /**
     * @brief Process palm detection
     * @param frame Current camera frame
     * @return Rectangle of detected palm, empty if none found
     */
    cv::Rect detectPalm(cv::Mat &frame);

    /**
     * @brief Convert OpenCV Mat to QImage
     * @param mat OpenCV image
     * @return QImage for display
     */
    QImage matToQImage(const cv::Mat &mat);

    /**
     * @brief Update the score display
     */
    void updateScoreDisplay();
};

#endif // MAINWINDOW_H
