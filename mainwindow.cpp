#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      camera(nullptr),
      cameraTimer(nullptr),
      isCameraActive(false),
      isDetectionActive(false),
      gameScore(0),
      featureCount(0)
{
    ui->setupUi(this);

    // Configure sizes in the splitter
    QList<int> sizes;
    sizes << (width() * 0.8) << (width() * 0.2); // 80% for glWidget, 20% for right panel
    ui->mainSplitter->setSizes(sizes);

    // Connect menu actions
    connect(ui->actionNew_Game, &QAction::triggered, this, &MainWindow::startNewGame);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(ui->captureButton, &QPushButton::clicked, this, &MainWindow::on_captureButton_clicked);

    // Initialize camera
    initializeCamera();

    // Setup initial score display
    updateScoreDisplay();
}

MainWindow::~MainWindow()
{
    // Clean up camera resources
    if (cameraTimer)
    {
        cameraTimer->stop();
        delete cameraTimer;
    }

    if (camera)
    {
        if (camera->isOpened())
        {
            camera->release();
        }
        delete camera;
    }

    delete ui;
}

bool MainWindow::initializeCamera()
{
    // Initialize the camera
    camera = new cv::VideoCapture(0);

    if (!camera->isOpened())
    {
        ui->cameraStatus->setText("Error: Camera not available");
        ui->cameraView->setText("No camera");
        return false;
    }

    // Get camera properties
    int width = camera->get(cv::CAP_PROP_FRAME_WIDTH);
    int height = camera->get(cv::CAP_PROP_FRAME_HEIGHT);

    ui->cameraStatus->setText(QString("Camera active (%1 x %2)").arg(width).arg(height));

    // Start the camera timer
    cameraTimer = new QTimer(this);
    connect(cameraTimer, &QTimer::timeout, this, &MainWindow::updateCameraFrame);
    cameraTimer->start(33); // ~30 FPS

    isCameraActive = true;
    return true;
}

void MainWindow::updateCameraFrame()
{
    if (!camera || !camera->isOpened())
    {
        return;
    }

    cv::Mat frame;
    if (!camera->read(frame))
    {
        ui->cameraStatus->setText("Error reading from camera");
        return;
    }

    // Flip image horizontally for mirror effect
    cv::flip(frame, frame, 1);

    // Process frame - detect features or palm
    if (isDetectionActive)
    {
        // Process with feature detection
        featureCount = processFrame(frame);

        // Update UI with feature count
        ui->matchInfo->setText(QString("Features: %1").arg(featureCount));

        // Update progress bar (assuming max of 100 features is good)
        int quality = qMin(100, featureCount);
        ui->matchQuality->setValue(quality);
    }
    else
    {
        // Detect palm
        cv::Rect palmRect = detectPalm(frame);
        if (!palmRect.empty())
        {
            detectionArea = palmRect;
            ui->cameraStatus->setText("Palm detected - Click 'Start Detection'");
            ui->captureButton->setEnabled(true);
        }
        else
        {
            ui->cameraStatus->setText("No palm detected");
            ui->captureButton->setEnabled(false);
        }
    }

    // Convert to QImage and display
    QImage img = matToQImage(frame);

    // Display with proper aspect ratio
    ui->cameraView->setPixmap(QPixmap::fromImage(img).scaled(
        ui->cameraView->width(),
        ui->cameraView->height(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
}

int MainWindow::processFrame(cv::Mat &frame)
{
    // Create SIFT detector
    cv::Ptr<cv::SIFT> detector = cv::SIFT::create();

    // Detect features
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    // Extract keypoints and descriptors
    detector->detectAndCompute(frame, cv::noArray(), keypoints, descriptors);

    // Draw keypoints on the image for visualization
    cv::drawKeypoints(frame, keypoints, frame, cv::Scalar(0, 255, 0),
                      cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    // Add debug text with feature count
    cv::putText(frame,
                "Features: " + std::to_string(keypoints.size()),
                cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7,
                cv::Scalar(0, 255, 0),
                2);

    return keypoints.size();
}

cv::Rect MainWindow::detectPalm(cv::Mat &frame)
{
    cv::CascadeClassifier palmCascade;
    if (!palmCascade.load("../../palm.xml"))
    {
        qDebug() << "Error loading palm cascade file";
        ui->cameraStatus->setText("Error: Couldn't load palm detector");
        return cv::Rect();
    }

    cv::Mat frameGray;
    std::vector<cv::Rect> palms;

    // Convert to grayscale
    cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);

    // Try to detect palm
    palmCascade.detectMultiScale(
        frameGray,
        palms,
        1.1,
        8,
        0,
        cv::Size(80, 80),
        cv::Size(250, 250));

    // If no detection, try inverted image
    if (palms.empty())
    {
        cv::Mat invertedGray = 255 - frameGray;
        palmCascade.detectMultiScale(
            invertedGray,
            palms,
            1.1,
            4,
            0,
            cv::Size(80, 80),
            cv::Size(250, 250));
    }

    // Draw rectangle around detected palm
    if (!palms.empty())
    {
        cv::rectangle(frame, palms[0], cv::Scalar(0, 255, 0), 2);
        return palms[0];
    }

    return cv::Rect();
}

QImage MainWindow::matToQImage(const cv::Mat &mat)
{
    // Convert OpenCV Mat to QImage for display

    // For color images
    if (mat.type() == CV_8UC3)
    {
        // OpenCV uses BGR, Qt needs RGB
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        return QImage(
                   rgbMat.data,
                   rgbMat.cols,
                   rgbMat.rows,
                   rgbMat.step,
                   QImage::Format_RGB888)
            .copy();
    }
    // For grayscale images
    else if (mat.type() == CV_8UC1)
    {
        return QImage(
                   mat.data,
                   mat.cols,
                   mat.rows,
                   mat.step,
                   QImage::Format_Grayscale8)
            .copy();
    }

    return QImage(); // Return invalid image if format not supported
}

void MainWindow::updateScoreDisplay()
{
    ui->scoreValue->setText(QString::number(gameScore));
}

void MainWindow::startNewGame()
{
    // Reset game parameters
    gameScore = 0;
    updateScoreDisplay();

    // Reset detection if active
    isDetectionActive = false;
    ui->captureButton->setText("Start Detection");
    ui->matchInfo->setText("Waiting for detection...");
    ui->matchQuality->setValue(0);

    QMessageBox::information(this, tr("New Game"), tr("Starting a new game. Get ready!"));
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, tr("About Slice Defender"),
                       tr("Slice Defender v1.0\n\n"
                          "A game that uses computer vision to detect and track hand movements.\n\n"
                          "Created with Qt and OpenCV."));
}

void MainWindow::on_captureButton_clicked()
{
    if (!isDetectionActive)
    {
        // Start detection mode
        isDetectionActive = true;
        ui->captureButton->setText("Stop Detection");
        ui->cameraStatus->setText("Feature detection active");
    }
    else
    {
        // Stop detection mode
        isDetectionActive = false;
        ui->captureButton->setText("Start Detection");
        ui->cameraStatus->setText("Detection stopped");
        ui->matchQuality->setValue(0);
    }
}
