#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include "opencv2/opencv.hpp"
#include <QWidget>
#include <QTimer>

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

private:
    Ui::CameraHandler *ui;
    VideoCapture *webCam_;
    Mat reference;
    bool hasReference;
    QTimer *timer;
    Rect lastDetectedRect;
    bool hasDetection;
    void siftMatching(Mat &image1, Mat &image2);
    Rect haarCascade(Mat &image);

private slots:
    void on_captureButton__clicked();
    void updateFrame();
};

#endif // CAMERAHANDLER_H
