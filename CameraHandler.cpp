#include "opencv2/opencv.hpp"
#include "CameraHandler.h"
#include "ui_CameraHandler.h"
#include <QString>
#include <QImage>
#include <iostream>

using namespace cv;
using namespace std;

/**
 * Constructor initializes webcam capture and UI components
 */
CameraHandler::CameraHandler(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::CameraHandler)
{
    ui->setupUi(this);

    webCam_ = new VideoCapture(1);
    hasReference = false;
    hasDetection = false;
    consecutiveDetections = 0;
    matchQuality = 0;
    detectionTimer.start();
    debug = true; // Set debug to true by default

    // Initialize hand position to middle of the camera frame
    int width = webCam_->get(CAP_PROP_FRAME_WIDTH);
    int height = webCam_->get(CAP_PROP_FRAME_HEIGHT);

    // Initialize to middle of frame or reasonable fallback values
    m_handPosition[0] = width > 0 ? width / 2 : 320;
    m_handPosition[1] = height > 0 ? height / 2 : 240;

    if (!webCam_->isOpened())
    {
        ui->detectionLabel_->setText("Error opening the default camera !");
    }
    else
    {
        ui->detectionLabel_->setText(QString("Video ok, image size is %1x%2 pixels").arg(width).arg(height));
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
        timer->start(30);
    }
}

/**
 * Destructor cleans up resources
 */
CameraHandler::~CameraHandler()
{
    timer->stop();
    delete timer;
    delete ui;
    delete webCam_;
}

/**
 * Implements hand detection using Haar cascade classifiers
 * Tries multiple detection approaches: fist, palm
 */
Rect CameraHandler::haarCascade(Mat &image)
{
    CascadeClassifier fist_cascade;
    CascadeClassifier palm_cascade;

    // Load the Haar cascade classifier for hand detection
    if (!fist_cascade.load("../../hand.xml"))
    {
        cerr << "Error loading hand.xml" << endl;
        return Rect();
    }
    if (!palm_cascade.load("../../Hand.Cascade.1.xml"))
    {
        cerr << "Error loading palm.xml" << endl;
        return Rect();
    }

    Mat frame_gray;
    std::vector<Rect> fists;
    std::vector<Rect> invfists;
    std::vector<Rect> palms;
    std::vector<Rect> invPalms;

    cv::cvtColor(image, frame_gray, COLOR_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray); // Improve contrast for better detection

    // First try to detect fists
    fist_cascade.detectMultiScale(frame_gray, fists, 1.1, 13, 1, Size(60, 60), Size(220, 220));

    Mat invFrame_gray = 255 - frame_gray; // Invert image for better palm detection
    palm_cascade.detectMultiScale(invFrame_gray, invfists, 1.1, 13, 1, Size(60, 60), Size(220, 220));
    fists.insert(fists.end(), invfists.begin(), invfists.end());

    // Second attempt: detect palms if no fists found
    if (fists.size() <= 0)
    {
        palm_cascade.detectMultiScale(frame_gray, palms, 1.1, 13, 1, Size(60, 60), Size(220, 220));
        // try inverted image for palm detection
        palm_cascade.detectMultiScale(invFrame_gray, invPalms, 1.1, 13, 1, Size(60, 60), Size(220, 220));
        palms.insert(palms.end(), invPalms.begin(), invPalms.end());
    }

    Rect detectedRect;

    // Prioritize fist detection over palm detection
    if (fists.size() > 0)
    {
        detectedRect = fists[0];
    }
    else if (palms.size() > 0)
    {
        detectedRect = palms[0];
    }

    // Draw detection rectangle only during initial detection phase
    if (!hasReference && !detectedRect.empty())
    {
        rectangle(image, detectedRect, Scalar(0, 255, 0), 2);
    }

    return detectedRect;
}

/**
 * Captures and processes a reference image for later matching
 * Applies targeted cropping to focus on the relevant part of the hand
 */
void CameraHandler::captureReference()
{
    if (webCam_->isOpened() && hasDetection)
    {
        Mat frame;
        if (webCam_->read(frame))
        {
            flip(frame, frame, 1); // Mirror image for natural interaction

            // Get frame dimensions for boundary checking
            int frameWidth = frame.cols;
            int frameHeight = frame.rows;

            // Safety check - ensure the detection rectangle is within the frame boundaries
            Rect safeRect = lastDetectedRect;
            safeRect.x = std::max(0, std::min(frameWidth - 1, safeRect.x));
            safeRect.y = std::max(0, std::min(frameHeight - 1, safeRect.y));
            safeRect.width = std::min(frameWidth - safeRect.x, safeRect.width);
            safeRect.height = std::min(frameHeight - safeRect.y, safeRect.height);

            // Skip if rectangle is too small after safety adjustments
            if (safeRect.width < 10 || safeRect.height < 10)
            {
                std::cout << "Warning: Adjusted rectangle too small for reference image" << std::endl;
                return;
            }

            // Adjust detection rectangle to better focus on the hand
            Rect adjustedRect = safeRect;
            adjustedRect.y = std::min(frameHeight - 1, adjustedRect.y + static_cast<int>(adjustedRect.height * 0.2));

            // Ensure adjusted height doesn't go beyond frame boundary
            adjustedRect.height = std::min(frameHeight - adjustedRect.y, adjustedRect.height);

            // Calculate crop dimensions to focus on central part of hand
            int cropX = static_cast<int>(adjustedRect.width * 0.3);
            int cropY = static_cast<int>(-adjustedRect.height * 0.2);

            // Ensure cropY doesn't move the rectangle outside the frame
            cropY = std::max(-adjustedRect.y, cropY);

            // Define final rectangle for reference image with boundary checks
            Rect finalRect(
                std::max(0, adjustedRect.x + cropX),
                std::max(0, adjustedRect.y + cropY),
                std::min(frameWidth - (adjustedRect.x + cropX), adjustedRect.width - 2 * cropX),
                std::min(frameHeight - (adjustedRect.y + cropY), adjustedRect.height - 2 * cropY));

            // Ensure the final rectangle is not empty or too small
            if (finalRect.width <= 0 || finalRect.height <= 0 ||
                finalRect.width < 10 || finalRect.height < 10)
            {
                std::cout << "Warning: Invalid reference rectangle dimensions" << std::endl;
                return;
            }

            try
            {
                // Extract region of interest and store as reference
                Mat roi = frame(finalRect);
                reference = roi.clone();
                hasReference = true;
                updateStatusText();

                // Display reference image when in debug mode
                if (debug)
                {
                    namedWindow("Reference Image", WINDOW_NORMAL);
                    imshow("Reference Image", reference);
                    resizeWindow("Reference Image", reference.cols, reference.rows);
                    waitKey(1); // Refresh the window
                }
            }
            catch (const cv::Exception &e)
            {
                std::cerr << "OpenCV error in captureReference: " << e.what() << std::endl;
                hasReference = false;
            }
        }
    }
}

/**
 * Updates the UI status text based on current detection phase
 */
void CameraHandler::updateStatusText()
{
    if (!hasDetection)
    {
        ui->detectionLabel_->setText("...");
    }
    else if (!hasReference)
    {
        ui->detectionLabel_->setText(QString("%1/%2").arg(consecutiveDetections).arg(REQUIRED_DETECTIONS));
    }
    else
    {
        ui->detectionLabel_->setText(QString("%1% sift").arg(matchQuality));
    }
}

/**
 * Determines if two detections are spatially close to each other
 * Uses center point distance relative to detection size
 */
bool CameraHandler::isDetectionClose(const Rect &current, const Rect &previous)
{
    // Calculate the centers of both rectangles
    Point currentCenter(current.x + current.width / 2, current.y + current.height / 2);
    Point previousCenter(previous.x + previous.width / 2, previous.y + previous.height / 2);

    // Calculate the Euclidean distance between centers
    double distance = sqrt(pow(currentCenter.x - previousCenter.x, 2) +
                           pow(currentCenter.y - previousCenter.y, 2));

    // Calculate the average size of the rectangles
    double avgSize = (current.width + current.height + previous.width + previous.height) / 4.0;

    // Consider it close if the distance is less than 30% of the average size
    return distance < (0.3 * avgSize);
}

/**
 * Main processing function for each video frame
 * Handles detection, tracking, and UI updates
 */
void CameraHandler::updateFrame()
{
    if (webCam_->isOpened())
    {
        Mat frame;
        if (webCam_->read(frame))
        {
            flip(frame, frame, 1); // Mirror image for natural interaction
            Mat frameToDisplay = frame.clone();

            // Phase 1: Hand detection and reference image capture
            if (!hasReference)
            {
                Rect detected = haarCascade(frameToDisplay);
                if (detected.width > 0 && detected.height > 0)
                {
                    bool isClose = false;
                    if (hasDetection)
                    {
                        isClose = isDetectionClose(detected, lastDetectedRect);
                    }

                    // Check if the new detection is close to the previous one or if this is the first detection
                    if (isClose || !hasDetection)
                    {
                        // Check if we haven't timed out (5 seconds since last detection)
                        if (detectionTimer.elapsed() < 5000 || !hasDetection)
                        {
                            consecutiveDetections++;
                        }
                        else
                        {
                            // It's been too long since the last detection, reset counter
                            consecutiveDetections = 1;
                        }
                    }
                    else
                    {
                        // Detection is not close to the previous one, reset counter
                        consecutiveDetections = 1;
                    }

                    lastDetectedRect = detected;
                    hasDetection = true;
                    detectionTimer.restart();

                    // Log detection progress
                    std::cout << consecutiveDetections << "/" << REQUIRED_DETECTIONS << std::endl;

                    // Capture reference once we have enough consistent detections
                    if (consecutiveDetections >= REQUIRED_DETECTIONS)
                    {
                        captureReference();
                        std::cout << "Reference captured!" << std::endl;
                    }
                }
                else
                {
                    // Reset if no detection for too long
                    if (detectionTimer.elapsed() > 5000 && consecutiveDetections > 0)
                    {
                        std::cout << "No detection for too long, resetting counter" << std::endl;
                        consecutiveDetections = 0;
                        hasDetection = false;
                    }
                }

                updateStatusText();
            }
            // Phase 2: Feature matching and tracking
            else
            {
                // Store original frame before adding annotations
                Mat originalFrame = frameToDisplay.clone();

                Rect detected = haarCascade(originalFrame);
                frameToDisplay = originalFrame.clone();

                if (detected.width > 0 && detected.height > 0)
                {
                    lastDetectedRect = detected;

                    // Ensure the detected rectangle is within frame boundaries
                    int frameWidth = frameToDisplay.cols;
                    int frameHeight = frameToDisplay.rows;

                    Rect safeRect = lastDetectedRect;
                    safeRect.x = std::max(0, std::min(frameWidth - 1, safeRect.x));
                    safeRect.y = std::max(0, std::min(frameHeight - 1, safeRect.y));
                    safeRect.width = std::min(frameWidth - safeRect.x, safeRect.width);
                    safeRect.height = std::min(frameHeight - safeRect.y, safeRect.height);

                    // Skip processing if the rectangle is too small after adjustments
                    if (safeRect.width < 10 || safeRect.height < 10)
                    {
                        std::cout << "Warning: Adjusted rectangle too small for feature matching" << std::endl;
                        return;
                    }

                    // Use safe rectangle instead of potentially unsafe lastDetectedRect
                    try
                    {
                        Mat currentFrame = frameToDisplay(safeRect).clone();

                        // Apply SIFT matching with reference image
                        std::vector<KeyPoint> keypoints = applySIFT(reference, currentFrame);

                        // Draw detection rectangle
                        rectangle(frameToDisplay, safeRect, Scalar(0, 255, 0), 1);

                        // Fallback to center point if SIFT matching quality is poor
                        if (matchQuality < 5)
                        {
                            static int lowQualityCounter = 0;
                            lowQualityCounter++;

                            // Draw the middle point of the detection as fallback
                            Point centerPoint(safeRect.x + safeRect.width / 2,
                                              safeRect.y + safeRect.height / 2);

                            // Update the tracked hand position
                            setTrackedHandPosition(centerPoint.x, centerPoint.y);

                            // Draw a red circle at the tracking point for visibility
                            circle(frameToDisplay, centerPoint, 5, Scalar(0, 0, 255), -1);

                            std::cout << "Low match quality, using detection center. Counter: "
                                      << lowQualityCounter << "/10" << std::endl;

                            // Reset reference if consistently poor matches
                            if (lowQualityCounter > 10)
                            {
                                std::cout << "Consistently poor matches, capturing new reference..." << std::endl;
                                hasReference = false;
                                consecutiveDetections = 0;
                                lowQualityCounter = 0;
                            }
                        }
                        else
                        {
                            // Good SIFT match - reset counter and draw keypoints
                            static int lowQualityCounter = 0;
                            lowQualityCounter = 0;

                            // Calculate average position of keypoints for better tracking
                            Point avgPoint(0, 0);
                            if (keypoints.size() > 0)
                            {
                                for (const KeyPoint &kp : keypoints)
                                {
                                    avgPoint.x += safeRect.x + kp.pt.x;
                                    avgPoint.y += safeRect.y + kp.pt.y;
                                }
                                avgPoint.x /= keypoints.size();
                                avgPoint.y /= keypoints.size();

                                // Update the tracked hand position
                                setTrackedHandPosition(avgPoint.x, avgPoint.y);

                                // Draw a red circle at the tracking point
                                circle(frameToDisplay, avgPoint, 5, Scalar(0, 0, 255), -1);
                            }
                            else
                            {
                                // Fallback to center of detection rectangle if no keypoints
                                Point centerPoint(safeRect.x + safeRect.width / 2,
                                                  safeRect.y + safeRect.height / 2);
                                setTrackedHandPosition(centerPoint.x, centerPoint.y);
                                circle(frameToDisplay, centerPoint, 5, Scalar(0, 0, 255), -1);
                            }

                            // Visualize keypoints
                            for (const KeyPoint &kp : keypoints)
                            {
                                // Adjust keypoint position to be relative to the detected rectangle
                                Point pt(safeRect.x + kp.pt.x, safeRect.y + kp.pt.y);
                                // Check if the point is within the frame boundaries
                                if (pt.x >= 0 && pt.x < frameWidth && pt.y >= 0 && pt.y < frameHeight)
                                {
                                    circle(frameToDisplay, pt, 2, Scalar(0, 255, 0), -1);
                                }
                            }
                        }
                    }
                    catch (const cv::Exception &e)
                    {
                        std::cerr << "OpenCV error in updateFrame: " << e.what() << std::endl;
                    }

                    std::cout << "Match: " << matchQuality << "%" << std::endl;
                }
                else
                {
                    // Reset match quality when no detection
                    if (matchQuality > 0)
                    {
                        std::cout << "Match: 0%" << std::endl;
                        matchQuality = 0;
                        updateStatusText();
                    }

                    // Reset reference if consistently can't detect anything
                    static int noDetectionCounter = 0;
                    noDetectionCounter++;

                    if (noDetectionCounter > 60)
                    {
                        std::cout << "No detection for too long, resetting reference..." << std::endl;
                        hasReference = false;
                        consecutiveDetections = 0;
                        noDetectionCounter = 0;
                    }
                }
            }

            Mat displayFrame;
            cvtColor(frameToDisplay, displayFrame, COLOR_BGR2RGB);
            QImage img = QImage((const unsigned char *)(displayFrame.data),
                                displayFrame.cols, displayFrame.rows,
                                QImage::Format_RGB888);

            // Scale image while preserving aspect ratio
            ui->imageLabel_->setPixmap(QPixmap::fromImage(img).scaled(
                ui->imageLabel_->width(),
                ui->imageLabel_->height(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));

            ui->imageLabel_->setAlignment(Qt::AlignCenter);
        }
    }
}

/**
 * Rotates an image by the specified angle
 */
Mat CameraHandler::rotateImage(const Mat &src, float angle)
{
    // Calculate image center
    Point2f center(src.cols / 2.0f, src.rows / 2.0f);

    // Create rotation matrix
    Mat rotMatrix = getRotationMatrix2D(center, angle, 1.0);

    // Apply rotation
    Mat result;
    warpAffine(src, result, rotMatrix, src.size());

    return result;
}

/**
 * Performs SIFT feature matching between two images
 * Updates the global matchQuality value and emits signal for UI updates
 */
std::vector<KeyPoint> CameraHandler::applySIFT(Mat &image1, Mat &image2)
{
    // Check if input images are valid
    if (image1.empty() || image2.empty())
    {
        std::cerr << "Empty images provided to SIFT matcher" << std::endl;
        matchQuality = 0;
        updateStatusText();
        return std::vector<KeyPoint>();
    }

    try
    {
        Mat img1 = image1;
        Mat img2 = image2;
        Ptr<SIFT> detector = SIFT::create();
        std::vector<KeyPoint> keypoints1, keypoints2;
        Mat descriptors1, descriptors2;
        detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
        detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

        // Check if descriptors are empty or not enough keypoints
        if (descriptors1.empty() || descriptors2.empty() || keypoints1.size() < 5 || keypoints2.size() < 5)
        {
            matchQuality = 0;
            updateStatusText();
            return keypoints2;
        }

        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
        std::vector<std::vector<DMatch>> knn_matches;

        // Check if we have enough matches to do knnMatch with k=2
        if (descriptors1.rows < 2 || descriptors2.rows < 2)
        {
            // Not enough descriptors for knnMatch with k=2
            // Fall back to simple match
            std::vector<DMatch> simple_matches;
            matcher->match(descriptors1, descriptors2, simple_matches);
            matchQuality = min(100, static_cast<int>(simple_matches.size() * 100.0 / max(1, static_cast<int>(keypoints1.size()))));
            updateStatusText();
            return keypoints2;
        }

        matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);

        // Apply ratio test to find good matches
        std::vector<DMatch> good_matches;
        for (size_t i = 0; i < knn_matches.size(); i++)
        {
            if (knn_matches[i].size() > 1)
            {
                if (knn_matches[i][0].distance < m_siftRationTresh * knn_matches[i][1].distance)
                {
                    good_matches.push_back(knn_matches[i][0]);
                }
            }
        }

        // Calculate match quality as a percentage (0-100)
        matchQuality = min(100, static_cast<int>(good_matches.size() * 100.0 / max(1, static_cast<int>(keypoints1.size()))));

        updateStatusText();
        return keypoints2;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "OpenCV error in applySIFT: " << e.what() << std::endl;
        matchQuality = 0;
        updateStatusText();
        return std::vector<KeyPoint>();
    }
}

/**
 * @brief Get the hand/sword position detected by the camera
 * @return Normalized 3D vector between (-1,-1,0) and (1,1,0)
 *
 * This method converts the detected hand position to normalized coordinates
 * where (-1,-1,0) is the bottom-left corner of the screen,
 * (0,0,0) is the center, and (1,1,0) is the top-right corner.
 * If no hand is detected, returns the last valid position instead of (0,0,0).
 */
QVector3D CameraHandler::getHandPosition() const
{
    // Get frame dimensions
    int frameWidth = webCam_->get(cv::CAP_PROP_FRAME_WIDTH);
    int frameHeight = webCam_->get(cv::CAP_PROP_FRAME_HEIGHT);

    // Use the tracked hand position
    float handX_px = m_handPosition[0];
    float handY_px = m_handPosition[1];

    // Normalize to -1 to 1 range
    // X: -1 (left) to 1 (right)
    // Y: -1 (bottom) to 1 (top)
    float normalizedX = 2.0f * (handX_px / frameWidth) - 1.0f;
    float normalizedY = 1.0f - 2.0f * (handY_px / frameHeight); // Invert Y axis

    // Create QVector3D with Z=0 (2D tracking)
    return QVector3D(normalizedX, normalizedY, 0.0f);
}

/**
 * @brief Set the tracked hand position
 * @param x X-coordinate of the hand position
 * @param y Y-coordinate of the hand position
 *
 * Updates the internal tracking of hand position that controls the player's sword.
 */
void CameraHandler::setTrackedHandPosition(int x, int y)
{
    m_handPosition[0] = x;
    m_handPosition[1] = y;
}
