#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include "opencv2/opencv.hpp"
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector3D>

using namespace cv;

namespace Ui
{
    class CameraHandler;
}

/**
 * @brief The CameraHandler class manages webcam operations for hand detection and tracking
 *
 * This class provides functionality to:
 * - Capture video from a webcam
 * - Detect hand positions using Haar cascades
 * - Establish a reference image after consistent detection
 * - Track hand position using SIFT feature matching
 */
class CameraHandler : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor initializes webcam and UI components
     * @param parent The parent widget
     */
    explicit CameraHandler(QWidget *parent = 0);

    /**
     * @brief Destructor releases resources like timer and webcam
     */
    ~CameraHandler();

    /**
     * @brief Get the hand/sword position detected by the camera
     * @return Normalized 3D vector between (-1,-1,0) and (1,1,0)
     */
    QVector3D getHandPosition() const;

private:
    Ui::CameraHandler *ui;
    VideoCapture *webCam_;
    Mat reference;
    bool hasReference;
    QTimer *timer;
    Rect lastDetectedRect;
    bool hasDetection;
    int consecutiveDetections;
    static const int REQUIRED_DETECTIONS = 15;
    int matchQuality;
    QElapsedTimer detectionTimer;
    bool debug;
    const float m_siftRationTresh = 0.85f;
    bool isDetectionClose(const Rect &current, const Rect &previous);

    /**
     * @brief Performs SIFT feature matching between two images
     * @param image1 Reference image
     * @param image2 Current image for comparison
     * @return Vector of keypoints from the matched image
     */
    std::vector<KeyPoint> applySIFT(Mat &image1, Mat &image2);

    /**
     * @brief Detects hand using Haar cascade classifiers
     * @param image Input image to process
     * @return Rectangle containing detected hand (empty if no detection)
     */
    Rect haarCascade(Mat &image);

    /**
     * @brief Captures reference image when hand is consistently detected
     */
    void captureReference();

    /**
     * @brief Updates UI status text based on current detection state
     */
    void updateStatusText();

    /**
     * @brief Rotates an image by a given angle
     * @param src Source image
     * @param angle Rotation angle in degrees
     * @return Rotated image
     */
    Mat rotateImage(const Mat &src, float angle);

private slots:
    /**
     * @brief Updates and processes each video frame
     * Handles detection, tracking, and UI updates
     */
    void updateFrame();
};

#endif // CAMERAHANDLER_H
