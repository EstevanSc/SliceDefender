#define _USE_MATH_DEFINES

#include <GL/glu.h>
#include <QApplication>
#include "myglwidget.h"
#include <cmath>
#include "cannon.h"

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    // Initialisation du timer pour les animations
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(16); // ~60 FPS
}

MyGLWidget::~MyGLWidget()
{
    // Nettoyage des ressources
    delete timer;
}

void MyGLWidget::initializeGL()
{
    // Initialisation des fonctions OpenGL
    initializeOpenGLFunctions();

    // Configuration de base
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // Fond plus sombre
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    // Configuration des matrices
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(QVector3D(0.0f, 2.5f, 0.0f),  // Position de la caméra montée
                      QVector3D(0.0f, 0.0f, -1.0f), // Direction de vue
                      QVector3D(0.0f, 1.0f, 0.0f)); // Vecteur "up"
}

void MyGLWidget::resizeGL(int width, int height)
{
    // Ajustement du viewport
    glViewport(0, 0, width, height);

    // Mise à jour de la matrice de projection
    float aspectRatio = static_cast<float>(width) / height;
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, aspectRatio, 0.1f, 100.0f);
}

void MyGLWidget::paintGL()
{
    // Effacement des buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Application des matrices de projection et de vue
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspectRatio = static_cast<float>(width()) / height();
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 2.5f, 0.0f,
              0.0f, 0.0f, -corridorLength,
              0.0f, 1.0f, 0.0f);

    // Configuration de l'éclairage
    setupLight();

    // Dessiner les axes de coordonnées (X, Y, Z)
    drawAxes();

    // Dessin des éléments de la scène
    drawCorridor();
    drawCannon();
    // drawCylindricalGrid();
}

void MyGLWidget::setupLight()
{
    // Configuration de la lumière directionnelle (vers le bas)
    GLfloat lightPosition[] = {0.0f, 10.0f, 0.0f, 0.0f}; // Directionnelle
    GLfloat lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

void MyGLWidget::drawCorridor()
{
    // Dessin du couloir
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f); // Gris

    // Sol
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-corridorWidth / 2, 0.0f, 0.0f);
    glVertex3f(corridorWidth / 2, 0.0f, 0.0f);
    glVertex3f(corridorWidth / 2, 0.0f, -corridorLength);
    glVertex3f(-corridorWidth / 2, 0.0f, -corridorLength);
    glEnd();

    // Plafond
    // glBegin(GL_QUADS);
    // glNormal3f(0.0f, -1.0f, 0.0f);
    // glVertex3f(-corridorWidth / 2, corridorHeight, 0.0f);
    // glVertex3f(-corridorWidth / 2, corridorHeight, -corridorLength);
    // glVertex3f(corridorWidth / 2, corridorHeight, -corridorLength);
    // glVertex3f(corridorWidth / 2, corridorHeight, 0.0f);
    // glEnd();

    // Mur gauche
    // glBegin(GL_QUADS);
    // glNormal3f(1.0f, 0.0f, 0.0f);
    // glVertex3f(-corridorWidth / 2, 0.0f, 0.0f);
    // glVertex3f(-corridorWidth / 2, 0.0f, -corridorLength);
    // glVertex3f(-corridorWidth / 2, corridotHeight, -corridorLength);
    // glVertex3f(-corridorWidth / 2, corridotHeight, 0.0f);
    // glEnd();

    // Mur droit
    // glBegin(GL_QUADS);
    // glNormal3f(-1.0f, 0.0f, 0.0f);
    // glVertex3f(corridorWidth / 2, 0.0f, 0.0f);
    // glVertex3f(corridorWidth / 2, corridotHeight, 0.0f);
    // glVertex3f(corridorWidth / 2, corridotHeight, -corridorLength);
    // glVertex3f(corridorWidth / 2, 0.0f, -corridorLength);
    // glEnd();

    glPopMatrix();
}

void MyGLWidget::drawCannon()
{
    // Dessin du canon
    glPushMatrix();

    // Utilisation de la classe Cannon pour dessiner le canon
    Cannon cannon;
    cannon.setPosition(QVector3D(0.0f, 0, -corridorLength));
    cannon.setAngle(-20.0f);
    cannon.setLength(3.0f);
    cannon.setRadius(0.8f);
    cannon.setColor(QColor(50, 50, 50));

    // Paramètres pour les roues
    cannon.setWheelRadius(0.7f);
    cannon.setWheelThickness(0.3f);

    cannon.draw();

    glPopMatrix();
}

void MyGLWidget::drawCylindricalGrid()
{
    // Dessin de la grille cylindrique (60 degrés)
    glPushMatrix();
    glColor3f(0.0f, 0.8f, 0.8f);    // Cyan
    glTranslatef(0.0f, 0.0f, 2.5f); // Rapproche la grille de la caméra

    GLUquadric *quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_LINE); // Mode filaire

    // Portion de cylindre (60°)
    const int segments = 15;
    const float angleStep = gridAngle / segments;
    const float startAngle = -gridAngle / 2;
    const float height = corridorHeight * 0.8f;

    glBegin(GL_LINES);

    // Lignes longitudinales
    for (int i = 0; i <= segments; i++)
    {
        float angle = (startAngle + i * angleStep) * M_PI / 180.0f;
        float x = gridRadius * sin(angle);
        float z = -gridRadius * cos(angle);

        glVertex3f(x, -height / 2, z);
        glVertex3f(x, height / 2, z);
    }

    // Lignes horizontales
    const int heightSegments = 10;
    for (int j = 0; j <= heightSegments; j++)
    {
        float y = -height / 2 + j * (height / heightSegments);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= segments; i++)
        {
            float angle = (startAngle + i * angleStep) * M_PI / 180.0f;
            float x = gridRadius * sin(angle);
            float z = -gridRadius * cos(angle);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    glEnd();

    gluDeleteQuadric(quad);
    glPopMatrix();
}

void MyGLWidget::drawAxes()
{
    // Désactiver temporairement l'éclairage pour que les axes soient bien visibles
    glDisable(GL_LIGHTING);

    const float axisLength = 2.0f; // Longueur des axes

    // Axe X en rouge
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // Rouge
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(axisLength, 0.0f, 0.0f);
    glEnd();

    // Axe Y en vert
    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f); // Vert
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, axisLength, 0.0f);
    glEnd();

    // Axe Z en bleu
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f); // Bleu
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, axisLength); // Z négatif pour aller dans la profondeur
    glEnd();

    // Réactiver l'éclairage
    glEnable(GL_LIGHTING);
}
