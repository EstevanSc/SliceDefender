#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      cameraHandler(nullptr),
      game(nullptr),
      gameScore(0),
      m_showingScoreboard(false),
      m_showingInstructions(false),
      m_standardMode(true) // Default to Standard Mode
{
    ui->setupUi(this);

    // Create scoreboard
    m_scoreboard = new Scoreboard(this);

    // Initialize status timer for temporary messages
    m_statusTimer = new QTimer(this);
    m_statusTimer->setSingleShot(true);
    connect(m_statusTimer, &QTimer::timeout, [this]()
            { ui->statusLabel->clear(); });

    // Initialize game timer
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &MainWindow::updateGameTimeDisplay);

    // Hide the scoreboard and instructions overlays initially
    ui->scoreboardOverlay->hide();
    ui->instructionsOverlay->hide();

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

    // Set default button text to show what mode will be switched to if clicked
    ui->modeSwitchButton->setText("Original Mode");
}

MainWindow::~MainWindow()
{
    // Stop the timer if it's running
    if (m_gameTimer->isActive())
    {
        m_gameTimer->stop();
    }

    delete cameraHandler;
    delete m_scoreboard;
    delete ui;
}

void MainWindow::startNewGame()
{
    // If scoreboard or instructions is showing, hide it first
    if (m_showingScoreboard)
    {
        toggleScoreboard();
    }
    else if (m_showingInstructions)
    {
        toggleInstructions();
    }

    startGame();
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, tr("About Slice Defender"),
                       tr("Slice Defender v1.0\n\n"
                          "A game that uses computer vision to detect and track hand movements.\n\n"
                          "Created with Qt and OpenCV."));
}

void MainWindow::startGame()
{
    if (game)
    {
        // Hide scoreboard or instructions if it's visible
        if (m_showingScoreboard)
        {
            toggleScoreboard();
        }
        else if (m_showingInstructions)
        {
            toggleInstructions();
        }

        // Reset the game state
        game->resetGame();

        // Ensure the game uses the correct mode
        game->setStandardMode(m_standardMode);

        // Hide the start button and show the countdown
        ui->startButton->hide();
        ui->countdownLabel->show();

        // Disable save button during gameplay
        ui->saveButton->setEnabled(false);

        // Release focus from name input to prevent accidental keyboard input capture
        ui->nameInput->clearFocus();
        ui->nameInput->clear();

        // Start the countdown
        game->startCountdown();

        // Set focus explicitly to the OpenGL widget for immediate keyboard control
        ui->glWidget->setFocus();

        // Force the widget to get focus
        ui->glWidget->grabKeyboard();

        // Reset game timer with centisecond precision
        m_gameStartTime = QTime::currentTime();
        ui->gameTimeLabel->setText("00:00:00");
    }
}

void MainWindow::updateScoreDisplay()
{
    ui->scoreValue->setText(QString("POINTS:\n%1").arg(gameScore));
}

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

void MainWindow::updateScoreLabel(int score)
{
    gameScore = score;
    updateScoreDisplay();
}

void MainWindow::updateLivesLabel(int lives)
{
    ui->livesLabel->setText(QString("Lives:\n%1").arg(lives));
}

void MainWindow::updateCountdownLabel(int value)
{
    if (value <= 0)
    {
        ui->countdownLabel->hide();

        // Start the game timer
        m_gameStartTime = QTime::currentTime();
        m_gameTimer->start(10);
    }
    else
    {
        ui->countdownLabel->setText(QString::number(value));
    }
}

void MainWindow::showStartButton()
{
    ui->startButton->setText("Restart Game");
    ui->startButton->show();

    // Always enable the input field and save button
    // regardless of high score status
    ui->saveButton->setEnabled(true);
    ui->nameInput->setEnabled(true);

    // Release keyboard focus from the GL widget
    ui->glWidget->releaseKeyboard();

    // Give focus to name input field for immediate typing
    ui->nameInput->clear();
    ui->nameInput->setFocus();

    // Stop the game timer
    m_gameTimer->stop();
    // Show Game Over message in red
    showStatusMessage("Game Over", false);
}

/**
 * @brief Updates the game timer display with centisecond precision
 *
 * Calculates elapsed time between current time and game start time,
 * displaying it in mm:ss:XX format (minutes:seconds:centiseconds).
 */
void MainWindow::updateGameTimeDisplay()
{
    // Calculate elapsed time since game start
    QTime currentTime = QTime::currentTime();
    int elapsedMs = m_gameStartTime.msecsTo(currentTime);

    // Convert to minutes, seconds, centiseconds
    int minutes = (elapsedMs / 60000) % 60;
    int seconds = (elapsedMs / 1000) % 60;
    int centiseconds = (elapsedMs / 10) % 100;

    // Create formatted string with leading zeros
    QString timeString = QString("%1:%2:%3")
                             .arg(minutes, 2, 10, QChar('0'))
                             .arg(seconds, 2, 10, QChar('0'))
                             .arg(centiseconds, 2, 10, QChar('0'));

    // Update the display
    ui->gameTimeLabel->setText(timeString);
}

void MainWindow::updateSpeedIndicator(float speedMultiplier)
{
    ui->speedLabel->setText(QString("Speed: %1x").arg(speedMultiplier, 0, 'f', 1));
}

void MainWindow::showStatusMessage(const QString &message, bool success)
{
    ui->statusLabel->setText(message);
    ui->statusLabel->setStyleSheet(success ? "color: #4CAF50; font-weight: bold;" : "color: #F44336; font-weight: bold;");

    // Auto-clear message after 3 seconds
    m_statusTimer->start(3000);
}

void MainWindow::toggleInstructions()
{
    m_showingInstructions = !m_showingInstructions;

    if (m_showingInstructions)
    {
        // Hide scoreboard if it's visible
        if (m_showingScoreboard)
        {
            m_showingScoreboard = false;
            ui->scoreboardOverlay->hide();
            ui->scoreboardButton->setText("Scoreboard");
        }

        // Show instructions overlay
        ui->instructionsButton->setText("Back to Game");
        ui->instructionsOverlay->show();
    }
    else
    {
        // Hide instructions overlay
        ui->instructionsButton->setText("Instructions");
        ui->instructionsOverlay->hide();

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

void MainWindow::toggleScoreboard()
{
    m_showingScoreboard = !m_showingScoreboard;

    if (m_showingScoreboard)
    {
        // Hide instructions if it's visible
        if (m_showingInstructions)
        {
            m_showingInstructions = false;
            ui->instructionsOverlay->hide();
            ui->instructionsButton->setText("Instructions");
        }

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

void MainWindow::toggleGameMode()
{
    // Only allow mode change when not in active gameplay
    if (game && game->isGameStarted())
    {
        showStatusMessage("Cannot change mode during active game", false);
        return;
    }

    // Toggle the mode
    m_standardMode = !m_standardMode;

    // Update button text
    if (m_standardMode)
    {
        ui->modeSwitchButton->setText("Original Mode");
    }
    else
    {
        ui->modeSwitchButton->setText("Standard Mode");
    }

    // Update game settings
    if (game)
    {
        game->setStandardMode(m_standardMode);
    }

    // Show confirmation message
    QString message = QString("Switched to %1").arg(m_standardMode ? "Standard Mode" : "Original Mode");
    showStatusMessage(message, true);
}
