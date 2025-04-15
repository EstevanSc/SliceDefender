#include "opencv2/opencv.hpp"
#include "CameraHandler.h"
#include "ui_CameraHandler.h"
#include <QString>
#include <QImage>
#include <iostream>

using namespace cv;
using namespace std;

CameraHandler::CameraHandler(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::CameraHandler)
{
    ui->setupUi(this);

    webCam_ = new VideoCapture(0);
    hasReference = false;
    hasDetection = false;
    consecutiveDetections = 0;
    matchQuality = 0;
    detectionTimer.start();

    int width = webCam_->get(CAP_PROP_FRAME_WIDTH);
    int height = webCam_->get(CAP_PROP_FRAME_HEIGHT);

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

CameraHandler::~CameraHandler()
{
    timer->stop();
    delete timer;
    delete ui;
    delete webCam_;
}

void CameraHandler::siftMatching(Mat &image1, Mat &image2)
{
    Mat img1 = image1;
    Mat img2 = image2;
    Ptr<SIFT> detector = SIFT::create();
    std::vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<DMatch>> knn_matches;
    matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);

    const float ratio_thresh = 0.7f;
    std::vector<DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    // Calculate match quality as a percentage (0-100)
    matchQuality = min(100, static_cast<int>(good_matches.size() * 100.0 / max(1, static_cast<int>(keypoints1.size()))));

    // Emit the signal to update the main window
    emit matchQualityChanged(matchQuality);

    updateStatusText();
}

Rect CameraHandler::haarCascade(Mat &image)
{
    CascadeClassifier face_cascade;
    if (!face_cascade.load("../../palm.xml"))
    {
        cerr << "Error loading haarcascade" << endl;
        return Rect();
    }

    Mat frame_gray;
    std::vector<Rect> faces;
    std::vector<Rect> invFaces;

    cv::cvtColor(image, frame_gray, COLOR_BGR2GRAY);

    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 8, 0, Size(80, 80), Size(250, 250));
    Mat invFrame_gray = 255 - frame_gray;
    face_cascade.detectMultiScale(invFrame_gray, invFaces, 1.1, 4, 0, Size(80, 80), Size(250, 250));
    if (faces.size() <= 0)
    {
        faces = invFaces;
    }
    Rect detectedRect;
    if (faces.size() > 0)
    {
        detectedRect = faces[0];
        rectangle(image, detectedRect, Scalar(0, 255, 0), 2);
    }

    // Removing this imshow to avoid extra windows
    // imshow("WebCam", image);
    // waitKey(1);

    return detectedRect;
}

void CameraHandler::captureReference()
{
    if (webCam_->isOpened() && hasDetection)
    {
        Mat frame;
        if (webCam_->read(frame))
        {
            flip(frame, frame, 1);

            Rect adjustedRect = lastDetectedRect;
            adjustedRect.y += adjustedRect.height / 2;

            int cropX = adjustedRect.width / 7;
            int cropY = -(adjustedRect.height * 0.2);

            Rect finalRect(
                adjustedRect.x + cropX,
                adjustedRect.y + cropY,
                adjustedRect.width - 2 * cropX,
                adjustedRect.height - 2 * cropY);

            Mat roi = frame(finalRect);
            reference = roi.clone();
            hasReference = true;
            updateStatusText();
        }
    }
}

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

void CameraHandler::updateFrame()
{
    if (webCam_->isOpened())
    {
        Mat frame;
        if (webCam_->read(frame))
        {
            flip(frame, frame, 1);
            Mat frameToDisplay = frame.clone();

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
                        // Check if we haven't timed out (2 seconds since last detection)
                        if (detectionTimer.elapsed() < 2000 || !hasDetection)
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

                    // Simple text output for consecutive detections
                    std::cout << consecutiveDetections << "/" << REQUIRED_DETECTIONS << std::endl;

                    // If we've reached the required number of consecutive detections, capture reference
                    if (consecutiveDetections >= REQUIRED_DETECTIONS)
                    {
                        captureReference();
                        std::cout << "Reference captured!" << std::endl;
                    }
                }
                else
                {
                    // Check if it's been more than 2 seconds without detection
                    if (detectionTimer.elapsed() > 2000 && consecutiveDetections > 0)
                    {
                        std::cout << "No detection for too long, resetting counter" << std::endl;
                        consecutiveDetections = 0;
                        hasDetection = false;
                    }
                }

                updateStatusText();
            }
            else
            {
                Rect detected = haarCascade(frameToDisplay);
                if (detected.width > 0 && detected.height > 0)
                {
                    lastDetectedRect = detected;
                    Mat currentFrame = frameToDisplay(lastDetectedRect).clone();
                    siftMatching(reference, currentFrame);

                    // Output match quality as a percentage
                    std::cout << "Match: " << matchQuality << "%" << std::endl;
                }
                else
                {
                    if (matchQuality > 0)
                    {
                        std::cout << "Match: 0%" << std::endl;
                    }
                    matchQuality = 0;
                    updateStatusText();
                }
            }

            // Affichage de l'image
            Mat displayFrame;
            cvtColor(frameToDisplay, displayFrame, COLOR_BGR2RGB);
            QImage img = QImage((const unsigned char *)(displayFrame.data),
                                displayFrame.cols, displayFrame.rows,
                                QImage::Format_RGB888);

            // Ajusté pour afficher l'image complète sans déformation
            ui->imageLabel_->setPixmap(QPixmap::fromImage(img).scaled(
                ui->imageLabel_->width(),
                ui->imageLabel_->height(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));

            // Centrer l'image dans le label
            ui->imageLabel_->setAlignment(Qt::AlignCenter);
        }
    }
}
