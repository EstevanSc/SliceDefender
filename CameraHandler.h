#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include "opencv2/opencv.hpp"
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>

using namespace cv;

namespace Ui
{
    class CameraHandler;
}

class CameraHandler : public QWidget
{
    Q_OBJECT

public:
    explicit CameraHandler(QWidget *parent = 0);
    ~CameraHandler();

signals:
    void matchQualityChanged(int quality);

private:
    Ui::CameraHandler *ui;
    VideoCapture *webCam_;
    Mat reference;
    bool hasReference;
    QTimer *timer;
    Rect lastDetectedRect;
    bool hasDetection;
    int consecutiveDetections;
    static const int REQUIRED_DETECTIONS = 5;
    int matchQuality;
    QElapsedTimer detectionTimer;
    bool isDetectionClose(const Rect &current, const Rect &previous);
    void siftMatching(Mat &image1, Mat &image2);
    Rect haarCascade(Mat &image);
    void captureReference();
    void updateStatusText();

private slots:
    void updateFrame();
};

#endif // CAMERAHANDLER_H
