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

    if (!webCam_->isOpened())
    {
        ui->infoLabel_->setText("Error opening the default camera !");
    }
    else
    {
        ui->infoLabel_->setText(QString("Video ok, image size is %1x%2 pixels").arg(width).arg(height));
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
        timer->start(30);
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
    Mat img_matches;
    drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, Scalar::all(-1),
                Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    imshow("Good Matches", img_matches);
    waitKey(1);
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
    std::vector<Rect> invFaces;

    cv::cvtColor(image, frame_gray, COLOR_BGR2GRAY);

    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 8, 0,Size(80,80), Size(250, 250));
    Mat invFrame_gray = 255-frame_gray;
    face_cascade.detectMultiScale(invFrame_gray, invFaces, 1.1, 4, 0,Size(80,80), Size(250, 250));
    if (faces.size()<=0){
        faces=invFaces;
    }
    Rect detectedRect;
    if (faces.size() > 0)
    {
        detectedRect = faces[0];
        rectangle(image, detectedRect, Scalar(0, 255, 0), 2);
    }

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
                Rect detected = haarCascade(frameToDisplay);
                if (detected.width > 0 && detected.height > 0)
                {
                    lastDetectedRect = detected;
                    hasDetection = true;
                }
            }
            else
            {
                siftMatching(reference, frameToDisplay);
            }

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
