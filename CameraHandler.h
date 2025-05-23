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
 * 
 * @author Estevan SCHMITT
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
     *
     * This method converts the detected hand position to normalized coordinates
     * where (-1,-1,0) is the bottom-left corner of the screen,
     * (0,0,0) is the center, and (1,1,0) is the top-right corner.
     * If no hand is detected, returns the last valid position instead of (0,0,0).
     */
    QVector3D getHandPosition() const;

    /**
     * @brief Get the current tracked hand position in screen coordinates
     * @return QPoint containing the x,y coordinates of the tracked hand
     */
    QPoint getTrackedHandPosition() const { return QPoint(m_handPosition[0], m_handPosition[1]); }

    /**
     * @brief Set the tracked hand position
     * @param x X-coordinate of the hand position
     * @param y Y-coordinate of the hand position
     *
     * Updates the internal tracking of hand position that controls the player's sword.
     */
    void setTrackedHandPosition(int x, int y);

private:
    Ui::CameraHandler *ui; // Pointer to the UI components
    VideoCapture *webCam_; // Pointer to the webcam capture object
    
    Mat reference; // Reference image for SIFT matching
    bool hasReference; // Flag indicating if a reference image has been captured
    
    QTimer *timer; // Timer for periodic updates
    QElapsedTimer detectionTimer; // Timer for detection duration
    
    Rect lastDetectedRect; // Last detected rectangle for hand position
    bool hasDetection; // Flag indicating if a hand has been detected
    int consecutiveDetections; // Count of consecutive detections
    
    static const int REQUIRED_DETECTIONS = 15; // Number of detections required to capture a reference image
    int matchQuality; // Quality of the SIFT match (0-100)
    
    
    bool debug; // Flag for enabling/disabling debug mode
    const float m_siftRationTresh = 0.85f; // SIFT ratio threshold for matching

    /**
     * @brief Stores the tracked hand position in screen coordinates
     * [0] = x-coordinate, [1] = y-coordinate
     */
    int m_handPosition[2];

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

    /**
     * @brief Updates and processes each video frame
     * Handles detection, tracking, and UI updates
     */
    void updateFrame();
};

#endif // CAMERAHANDLER_H
