#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

/**
 * @brief Constructor initializes UI and camera resources
 * @param parent Parent widget
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      cameraHandler(nullptr),
      game(nullptr),
      gameScore(0),
      m_showingScoreboard(false)
{
    ui->setupUi(this);

    // Create scoreboard
    m_scoreboard = new Scoreboard(this);

    // Initialize status timer for temporary messages
    m_statusTimer = new QTimer(this);
    m_statusTimer->setSingleShot(true);
    connect(m_statusTimer, &QTimer::timeout, [this]()
            { ui->statusLabel->clear(); });

    // Hide the scoreboard overlay initially
    ui->scoreboardOverlay->hide();

    // Configure splitter proportions (80% game view, 20% control panel)
    QList<int> sizes;
    sizes << (width() * 0.8) << (width() * 0.2);
    ui->mainSplitter->setSizes(sizes);

    // Connect action signals
    connect(ui->actionNew_Game, &QAction::triggered, this, &MainWindow::startNewGame);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    // Setup camera handler
    cameraHandler = new CameraHandler();
    ui->cameraLayout->insertWidget(0, cameraHandler);
    ui->cameraView->hide();
    ui->cameraStatus->hide();

    // Initialize UI states
    ui->countdownLabel->hide();

    // Get the OpenGL widget to access the player and projectile manager
    MyGLWidget *glWidget = qobject_cast<MyGLWidget *>(ui->glWidget);

    // Create the Game instance
    if (glWidget)
    {
        game = new Game(glWidget->getPlayer(),
                        cameraHandler,
                        glWidget->getKeyboardHandler(),
                        glWidget->getProjectileManager(),
                        this);

        // Connect game signals to UI slots
        connect(game, &Game::scoreChanged, this, &MainWindow::updateScoreLabel);
        connect(game, &Game::livesChanged, this, &MainWindow::updateLivesLabel);
        connect(game, &Game::countdownUpdated, this, &MainWindow::updateCountdownLabel);
        connect(game, &Game::gameEnded, this, &MainWindow::showStartButton);

        // Connect player position changes to the GL widget
        connect(game, &Game::playerPositionChanged, glWidget, &MyGLWidget::positionPlayerOnGrid);

        // Connect keyboard speed changes
        connect(glWidget->getKeyboardHandler(), &KeyboardHandler::speedMultiplierChanged,
                this, &MainWindow::updateSpeedIndicator);

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
    delete m_scoreboard;
    delete ui;
}

/**
 * @brief Start a new game with reset score
 */
void MainWindow::startNewGame()
{
    // If scoreboard is showing, hide it first
    if (m_showingScoreboard)
    {
        toggleScoreboard();
    }

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
        // Hide scoreboard if it's visible
        if (m_showingScoreboard)
        {
            toggleScoreboard();
        }

        // Reset the game state
        game->resetGame();

        // Hide the start button and show the countdown
        ui->startButton->hide();
        ui->countdownLabel->show();

        // Disable save button during gameplay
        ui->saveButton->setEnabled(false);

        // Start the countdown
        game->startCountdown();
    }
}

/**
 * @brief Updates the score display
 */
void MainWindow::updateScoreDisplay()
{
    ui->scoreValue->setText(QString("POINTS:\n%1").arg(gameScore));
}

/**
 * @brief Updates the scoreboard display with the latest scores
 */
void MainWindow::updateScoreboardDisplay()
{
    QStringList scores = m_scoreboard->loadTopScores();

    if (scores.isEmpty())
    {
        ui->scoreText->setText("No scores yet. Play a game!");
    }
    else
    {
        ui->scoreText->setText(scores.join("\n"));
    }
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
    ui->livesLabel->setText(QString("Lives:\n%1").arg(lives));
}

/**
 * @brief Updates the countdown display
 * @param value Current countdown value
 */
void MainWindow::updateCountdownLabel(int value)
{
    if (value <= 0)
    {
        ui->countdownLabel->hide();
    }
    else
    {
        ui->countdownLabel->setText(QString::number(value));
    }
}

/**
 * @brief Shows the start button when game ends
 */
void MainWindow::showStartButton()
{
    ui->startButton->setText("Restart Game");
    ui->startButton->show();

    // Enable save button if it's a high score
    ui->saveButton->setEnabled(m_scoreboard->isHighScore(gameScore));
}

/**
 * @brief Updates the speed indicator when the keyboard speed changes
 * @param speedMultiplier The new speed multiplier value
 */
void MainWindow::updateSpeedIndicator(float speedMultiplier)
{
    ui->speedLabel->setText(QString("Speed: %1x").arg(speedMultiplier, 0, 'f', 1));
}

/**
 * @brief Displays a status message in the scoreboard overlay
 * @param message The message to display
 * @param success Whether the operation was successful (green) or failed (red)
 */
void MainWindow::showStatusMessage(const QString &message, bool success)
{
    ui->statusLabel->setText(message);
    ui->statusLabel->setStyleSheet(success ? "color: #4CAF50; font-weight: bold;" : "color: #F44336; font-weight: bold;");

    // Auto-clear message after 3 seconds
    m_statusTimer->start(3000);
}

/**
 * @brief Toggles the scoreboard visibility
 */
void MainWindow::toggleScoreboard()
{
    m_showingScoreboard = !m_showingScoreboard;

    if (m_showingScoreboard)
    {
        // Show scoreboard overlay
        updateScoreboardDisplay();
        ui->scoreboardButton->setText("Back to Game");
        ui->scoreboardOverlay->show();
    }
    else
    {
        // Hide scoreboard overlay
        ui->scoreboardButton->setText("Scoreboard");
        ui->scoreboardOverlay->hide();

        // Make sure game controls are in the correct state
        if (game && game->isGameStarted())
        {
            ui->startButton->hide();
        }
        else
        {
            ui->startButton->show();
        }
    }
}

/**
 * @brief Saves the current score with the player's name
 */
void MainWindow::savePlayerScore()
{
    // Get the player name, default to "No Name" if empty
    QString playerName = ui->nameInput->text().trimmed();
    if (playerName.isEmpty())
    {
        playerName = "No Name";
    }

    // Save the score
    bool success = m_scoreboard->saveScore(playerName, gameScore);

    // Show success/failure message
    showStatusMessage(
        success ? "Score saved successfully!" : "Failed to save score",
        success);

    // Update the scoreboard display
    if (success)
    {
        updateScoreboardDisplay();

        // Clear the input field
        ui->nameInput->clear();

        // Disable save button to prevent multiple saves
        ui->saveButton->setEnabled(false);
    }
}
