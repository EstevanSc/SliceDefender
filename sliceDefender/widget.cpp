#include "opencv2/opencv.hpp"
#include "widget.h"
#include "ui_widget.h"
#include <QString>
#include <QImage>
#include <iostream>

using namespace cv;
using namespace std;

Widget::Widget(QWidget *parent) : QWidget(parent),
                                  ui(new Ui::Widget)
{
    ui->setupUi(this);

    webCam_ = new VideoCapture(1);
    hasReference = false;
    hasDetection = false;

    int width = webCam_->get(CAP_PROP_FRAME_WIDTH);
    int height = webCam_->get(CAP_PROP_FRAME_HEIGHT);

    if (!webCam_->isOpened()) // check if we succeeded
    {
        ui->infoLabel_->setText("Error opening the default camera !");
    }
    else
    {
        ui->infoLabel_->setText(QString("Video ok, image size is %1x%2 pixels").arg(width).arg(height));
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
        timer->start(30); // 30ms = ~33fps
    }
}

Widget::~Widget()
{
    timer->stop();
    delete timer;
    delete ui;
    delete webCam_;
}

void Widget::siftMatching(Mat &image1, Mat &image2)
{
    Mat img1 = image1;
    Mat img2 = image2;
    //-- Step 1: Detect the keypoints using SIFT Detector, compute the descriptors
    Ptr<SIFT> detector = SIFT::create();
    std::vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

    //-- Step 2: Matching descriptor vectors with a FLANN based matcher
    // Since SIFT is a floating-point descriptor NORM_L2 is used
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<DMatch>> knn_matches;
    matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);

    //-- Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.7f;
    std::vector<DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    //-- Draw matches
    Mat img_matches;
    drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, Scalar::all(-1),
                Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //-- Show detected matches
    imshow("Good Matches", img_matches);
    waitKey(1); // Changed to 1ms wait instead of infinite wait
}

Rect Widget::haarCascade(Mat &image)
{
    CascadeClassifier face_cascade;
    if (!face_cascade.load("../../palm.xml"))
    {
        cerr << "Error loading haarcascade" << endl;
        return Rect();
    }

    Mat frame_gray;
    std::vector<Rect> faces;

    // Convert to gray
    cv::cvtColor(image, frame_gray, COLOR_BGR2GRAY);

    //-- Detect faces
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 4, 0, Size(60, 60));

    Rect detectedRect;
    if (faces.size() > 0)
    {
        detectedRect = faces[0]; // On prend la première détection
        rectangle(image, detectedRect, Scalar(0, 255, 0), 2);
    }

    // Display frame
    imshow("WebCam", image);
    waitKey(1);

    return detectedRect;
}

void Widget::updateFrame()
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
                // Avant capture, utiliser haarCascade
                Rect detected = haarCascade(frameToDisplay);
                if (detected.width > 0 && detected.height > 0)
                {
                    lastDetectedRect = detected;
                    hasDetection = true;
                }
            }
            else
            {
                // Après capture, utiliser SIFT
                siftMatching(reference, frameToDisplay);
            }

            // Convertir pour affichage Qt
            Mat displayFrame;
            cvtColor(frameToDisplay, displayFrame, COLOR_BGR2RGB);
            QImage img = QImage((const unsigned char *)(displayFrame.data),
                                displayFrame.cols, displayFrame.rows,
                                QImage::Format_RGB888);
            ui->imageLabel_->setPixmap(QPixmap::fromImage(img));
            ui->imageLabel_->resize(ui->imageLabel_->pixmap().size());
        }
    }
}

void Widget::on_captureButton__clicked()
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
            int cropY = -(adjustedRect.height *0.2);

            Rect finalRect(
                adjustedRect.x + cropX,
                adjustedRect.y + cropY,
                adjustedRect.width - 2 * cropX,
                adjustedRect.height - 2 * cropY);

            Mat roi = frame(finalRect);
            reference = roi.clone();
            hasReference = true;
            ui->infoLabel_->setText("Reference ROI captured! Using SIFT matching...");
        }
    }
    else
    {
        ui->infoLabel_->setText("No detection available - cannot capture");
    }
}
