#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      cameraHandler(nullptr),
      gameScore(0)
{
    ui->setupUi(this);

    // Configure sizes in the splitter
    QList<int> sizes;
    sizes << (width() * 0.8) << (width() * 0.2); // 80% for glWidget, 20% for right panel
    ui->mainSplitter->setSizes(sizes);

    // Connect menu actions
    connect(ui->actionNew_Game, &QAction::triggered, this, &MainWindow::startNewGame);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    // Initialize CameraHandler
    cameraHandler = new CameraHandler();

    // Add the CameraHandler to the camera frame layout
    ui->cameraLayout->insertWidget(0, cameraHandler);

    // Hide the label placeholders from the UI, since CameraHandler provides its own
    ui->cameraView->hide();
    ui->cameraStatus->hide();

    // Connect signals to update match quality progress bar
    connect(cameraHandler, SIGNAL(matchQualityChanged(int)), this, SLOT(updateMatchQuality(int)));

    // Setup initial score display
    updateScoreDisplay();
}

MainWindow::~MainWindow()
{
    delete cameraHandler;
    delete ui;
}

// Update match quality progress bar when signal is received from CameraHandler
void MainWindow::updateMatchQuality(int quality)
{
    ui->matchQuality->setValue(quality);
    ui->matchInfo->setText(QString("Match quality: %1%").arg(quality));
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

    // Reset detection display
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
