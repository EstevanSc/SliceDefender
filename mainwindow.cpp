#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

/**
 * @brief Constructor initializes UI and camera resources
 * @param parent Parent widget
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      cameraHandler(nullptr),
      gameScore(0)
{
    ui->setupUi(this);

    // Configure splitter proportions (80% game view, 20% control panel)
    QList<int> sizes;
    sizes << (width() * 0.8) << (width() * 0.2);
    ui->mainSplitter->setSizes(sizes);

    connect(ui->actionNew_Game, &QAction::triggered, this, &MainWindow::startNewGame);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    cameraHandler = new CameraHandler();
    ui->cameraLayout->insertWidget(0, cameraHandler);

    ui->cameraView->hide();
    ui->cameraStatus->hide();

    updateScoreDisplay();
}

/**
 * @brief Destructor cleans up resources
 */
MainWindow::~MainWindow()
{
    delete cameraHandler;
    delete ui;
}

/**
 * @brief Update the match quality display
 * @param quality Match quality value
 */
void MainWindow::updateMatchQuality(int quality)
{
    ui->matchInfo->setText(QString("Detection in progress..."));
}

/**
 * @brief Update the score display
 */
void MainWindow::updateScoreDisplay()
{
    ui->scoreValue->setText(QString::number(gameScore));
}

/**
 * @brief Start a new game with reset score
 */
void MainWindow::startNewGame()
{
    // Reset game state
    gameScore = 0;
    updateScoreDisplay();

    ui->matchInfo->setText("Waiting for detection...");

    QMessageBox::information(this, tr("New Game"), tr("Starting a new game. Get ready!"));
}

/**
 * @brief Display the about dialog
 */
void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, tr("About Slice Defender"),
                       tr("Slice Defender v1.0\n\n"
                          "A game that uses computer vision to detect and track hand movements.\n\n"
                          "Created with Qt and OpenCV."));
}
