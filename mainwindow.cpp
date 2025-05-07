#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

/**
 * @brief Constructor initializes UI and camera resources
 * @param parent Parent widget
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      cameraHandler(nullptr),
      game(nullptr),
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

    // Create the game UI elements
    livesLabel = new QLabel("Lives: 5", this);
    livesLabel->setAlignment(Qt::AlignCenter);
    livesLabel->setFont(QFont("Arial", 12, QFont::Bold));
    livesLabel->setStyleSheet("color: red;");

    countdownLabel = new QLabel("5", this);
    countdownLabel->setAlignment(Qt::AlignCenter);
    countdownLabel->setFont(QFont("Arial", 36, QFont::Bold));
    countdownLabel->setStyleSheet("color: orange;");
    countdownLabel->hide();

    startButton = new QPushButton("Start Game", this);
    startButton->setFont(QFont("Arial", 12, QFont::Bold));
    startButton->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px;");
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);

    // Add them to the UI below the score
    ui->scoreLayout->addWidget(livesLabel);
    ui->scoreLayout->addWidget(countdownLabel);
    ui->scoreLayout->addWidget(startButton);

    // Get the OpenGL widget to access the player and projectile manager
    MyGLWidget *glWidget = qobject_cast<MyGLWidget *>(ui->glWidget);

    // Create the Game instance
    if (glWidget)
    {
        game = new Game(glWidget->getPlayer(),
                        cameraHandler,
                        glWidget->getProjectileManager(),
                        this);

        // Connect game signals to UI slots
        connect(game, &Game::scoreChanged, this, &MainWindow::updateScoreLabel);
        connect(game, &Game::livesChanged, this, &MainWindow::updateLivesLabel);
        connect(game, &Game::countdownUpdated, this, &MainWindow::updateCountdownLabel);
        connect(game, &Game::gameEnded, this, &MainWindow::showStartButton);

        // Set the game update function in the GL widget
        glWidget->setGameUpdateFunction([this]()
                                        {
            if (game) {
                game->update();
            } });
    }

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
 * @brief Start a new game with reset score
 */
void MainWindow::startNewGame()
{
    startGame();
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

/**
 * @brief Starts or restarts the game
 */
void MainWindow::startGame()
{
    if (game)
    {
        // Reset the game state
        game->resetGame();

        // Hide the start button and show the countdown
        startButton->hide();
        countdownLabel->show();

        // Start the countdown
        game->startCountdown();
    }
}

/**
 * @brief Updates the score display
 */
void MainWindow::updateScoreDisplay()
{
    ui->scoreValue->setText(QString::number(gameScore));
}

/**
 * @brief Updates the score display
 * @param score New score value
 */
void MainWindow::updateScoreLabel(int score)
{
    gameScore = score;
    updateScoreDisplay();
}

/**
 * @brief Updates the lives display
 * @param lives Remaining lives
 */
void MainWindow::updateLivesLabel(int lives)
{
    livesLabel->setText(QString("Lives: %1").arg(lives));
}

/**
 * @brief Updates the countdown display
 * @param value Current countdown value
 */
void MainWindow::updateCountdownLabel(int value)
{
    if (value <= 0)
    {
        countdownLabel->hide();
    }
    else
    {
        countdownLabel->setText(QString::number(value));
    }
}

/**
 * @brief Shows the start button when game ends
 */
void MainWindow::showStartButton()
{
    startButton->setText("Restart Game");
    startButton->show();
}
