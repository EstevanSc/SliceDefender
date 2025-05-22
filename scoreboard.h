#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QVector>

/**
 * @class Scoreboard
 * @brief Handles loading, saving, and displaying high scores
 *
 * The Scoreboard class provides functionality to:
 * - Load scores from a text file
 * - Save new scores
 * - Sort scores in descending order
 * - Provide access to top scores
 * 
 * @author Estevan SCHMITT
 */
class Scoreboard : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor initializes scoreboard and loads existing scores
     * @param parent Parent object
     */
    explicit Scoreboard(QObject *parent = nullptr);

    /**
     * @brief Save a player's score to the scoreboard file
     * @param playerName Player's name
     * @param score Player's score
     * @return true if save was successful, false otherwise
     */
    bool saveScore(const QString &playerName, int score);

    /**
     * @brief Load the top scores from the scoreboard file
     * @return List of top scores (format: "Name : Score")
     */
    QStringList loadTopScores();

    /**
     * @brief Check if a score qualifies for the high score list
     * @param score Score to check
     * @return true if the score qualifies, false otherwise
     */
    bool isHighScore(int score);

    /**
     * @brief Get visibility state of the scoreboard
     * @return true if scoreboard is visible
     */
    bool isVisible() const { return m_isVisible; }

    /**
     * @brief Set visibility of the scoreboard
     * @param visible true to show, false to hide
     */
    void setVisible(bool visible) { m_isVisible = visible; }

    /**
     * @brief Toggle scoreboard visibility
     */
    void toggleVisibility() { m_isVisible = !m_isVisible; }

private:
    static const int MAX_SCORES = 20;

    // Structure to store player scores
    struct PlayerScore
    {
        QString name;
        int score;
    };

    QVector<PlayerScore> m_scores;
    QString m_scoreboardFile;
    bool m_isVisible;

    void updateScoreboardDisplay();
    bool loadScores();
    void sortScores();
    bool saveAllScores();
};

#endif // SCOREBOARD_H
