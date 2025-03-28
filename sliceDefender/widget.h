#ifndef WIDGET_H
#define WIDGET_H

#include "opencv2/opencv.hpp"
#include <QWidget>
#include <QTimer>

using namespace cv;

namespace Ui
{
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
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

#endif // WIDGET_H
