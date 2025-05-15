#include "scoreboard.h"
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <algorithm>

/**
 * @brief Constructor initializes scoreboard and loads existing scores
 * @param parent Parent object
 */
Scoreboard::Scoreboard(QObject *parent)
    : QObject(parent),
      m_isVisible(false)
{
    // Set the scoreboard file path to the application directory
    m_scoreboardFile = QDir::currentPath() + "/Scoreboard.txt";

    // Load existing scores
    loadScores();
}

/**
 * @brief Save a player's score to the scoreboard file
 * @param playerName Player's name
 * @param score Player's score
 * @return true if save was successful, false otherwise
 */
bool Scoreboard::saveScore(const QString &playerName, int score)
{
    // Use default name if empty
    QString name = playerName.trimmed().isEmpty() ? "No Name" : playerName.trimmed();

    // Create a new score entry
    PlayerScore newScore;
    newScore.name = name;
    newScore.score = score;

    // Add new score to the list
    m_scores.append(newScore);

    // Sort scores
    sortScores();

    // Trim to max scores if needed
    while (m_scores.size() > MAX_SCORES)
    {
        m_scores.removeLast();
    }

    // Save to file
    return saveAllScores();
}

/**
 * @brief Load the top scores from the scoreboard file
 * @return List of top scores (format: "Name : Score")
 */
QStringList Scoreboard::loadTopScores()
{
    // Make sure scores are loaded and sorted
    loadScores();

    QStringList topScores;

    // Convert the scores to a string list
    for (int i = 0; i < m_scores.size() && i < MAX_SCORES; ++i)
    {
        QString scoreEntry = QString("%1. %2 : %3")
                                 .arg(i + 1, 2)
                                 .arg(m_scores[i].name)
                                 .arg(m_scores[i].score);
        topScores.append(scoreEntry);
    }

    return topScores;
}

/**
 * @brief Check if a score qualifies for the high score list
 * @param score Score to check
 * @return true if the score qualifies, false otherwise
 */
bool Scoreboard::isHighScore(int score)
{
    // Load existing scores
    loadScores();

    // If we don't have the maximum number of scores yet, any score qualifies
    if (m_scores.size() < MAX_SCORES)
    {
        return true;
    }

    // Check if the score is higher than the lowest score in the list
    return score > m_scores.last().score;
}

/**
 * @brief Load all scores from the scoreboard file
 * @return true if load was successful, false otherwise
 */
bool Scoreboard::loadScores()
{
    // Clear current scores
    m_scores.clear();

    // Check if file exists
    QFile file(m_scoreboardFile);
    if (!file.exists())
    {
        qDebug() << "Scoreboard file does not exist: " << m_scoreboardFile;
        return false;
    }

    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "Failed to open scoreboard file for reading:" << m_scoreboardFile;
        return false;
    }

    // Read scores
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if (parts.size() >= 2)
        {
            PlayerScore score;
            score.name = parts[0];
            score.score = parts[1].toInt();
            m_scores.append(score);
        }
    }

    file.close();

    // Sort scores
    sortScores();

    return true;
}

/**
 * @brief Sort scores in descending order
 */
void Scoreboard::sortScores()
{
    std::sort(m_scores.begin(), m_scores.end(),
              [](const PlayerScore &a, const PlayerScore &b)
              {
                  return a.score > b.score;
              });
}

/**
 * @brief Save all scores back to the scoreboard file
 * @return true if save was successful, false otherwise
 */
bool Scoreboard::saveAllScores()
{
    QFile file(m_scoreboardFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open scoreboard file for writing: " << m_scoreboardFile;
        return false;
    }

    QTextStream out(&file);
    for (const auto &score : m_scores)
    {
        out << score.name << "|" << score.score << "\n";
    }

    file.close();
    return true;
}
