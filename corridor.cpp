#include <GL/glu.h>
#include "corridor.h"
#include <cmath>
#include <QTime>

Corridor::Corridor(float length, float width, float height)
    : m_length(length), m_width(width), m_height(height)
{
    initializeOpenGLFunctions();
    float tileW = 0.9f, tileD = 0.9f;
    int cols = static_cast<int>(m_width / tileW);
    int rows = static_cast<int>(m_length / tileD);
}

void Corridor::draw()
{
    drawGround();
    drawRoof();
    drawWalls();
    drawTorches();
}

void Corridor::drawWalls()
{
    // Couleurs inversées : fond gris clair, tuiles gris foncé et gris moyen
    // Fond du mur (plan gris clair)
    QColor wallBaseColor(80, 80, 80); // gris clair
    GLfloat wallDiffuse[4] = {wallBaseColor.redF(), wallBaseColor.greenF(), wallBaseColor.blueF(), 1.0f};
    GLfloat wallAmbient[4] = {0.4f * wallBaseColor.redF(), 0.4f * wallBaseColor.greenF(), 0.4f * wallBaseColor.blueF(), 1.0f};
    GLfloat wallSpecular[4] = {0.12f, 0.12f, 0.12f, 1.0f};
    GLfloat wallShininess = 8.0f;
    // Mur gauche
    glPushMatrix();
    glTranslatef(-m_width / 2, 0, 0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, wallDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, wallAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wallSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, wallShininess);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, m_height, 0);
    glVertex3f(0, m_height, -m_length);
    glVertex3f(0, 0, -m_length);
    glEnd();
    glPopMatrix();
    // Mur droit
    glPushMatrix();
    glTranslatef(m_width / 2, 0, 0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, wallDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, wallAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wallSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, wallShininess);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, m_height, 0);
    glVertex3f(0, m_height, -m_length);
    glVertex3f(0, 0, -m_length);
    glEnd();
    glPopMatrix();
    // Motif pavé sur les murs latéraux avec 5 colonnes et offsets non linéaires
    float tileW = 0.9f, tileH = 0.6f, tileD = 0.04f;
    float gap = 0.025f;
    float gapY = 0.08f;
    int numCols = 5;
    int cols = static_cast<int>(m_length / tileW * numCols + 1);
    int rows = static_cast<int>(m_height / tileH + 1);
    QColor stoneColor1(140, 110, 60);
    QColor stoneColor2(170, 140, 80);

    // Offsets non linéaires pour chaque colonne (en proportion de tileH)
    float offsets[5] = {0.0f, 0.15f * tileH, 0.7f * tileH, 0.35f * tileH, 0.55f * tileH};

    // Mur gauche
    for (int y = -1; y < rows; ++y) {
        for (int c = 0; c < numCols; ++c) {
            for (int z = -1; z < cols / numCols; ++z) {
                float yPos = y * tileH + tileH / 2 + gap / 2 + offsets[c];
                float zPos = -((z * numCols + c) * tileW) - tileW / 2 - gapY / 2;
                QColor stoneColor = ((y + c + z) % 2 == 0) ? stoneColor1 : stoneColor2;
                float stoneLength = tileW - gapY;
                drawBrick(QVector3D(-m_width / 2 + tileD / 2 + 0.01f, yPos, zPos), tileD - gap, tileH - gap, stoneLength, stoneColor);
            }
        }
    }
    // Mur droit
    for (int y = -1; y < rows; ++y) {
        for (int c = 0; c < numCols; ++c) {
            for (int z = -1; z < cols / numCols; ++z) {
                float yPos = y * tileH + tileH / 2 + gap / 2 + offsets[c];
                float zPos = -((z * numCols + c) * tileW) - tileW / 2 - gapY / 2;
                QColor stoneColor = ((y + c + z) % 2 == 0) ? stoneColor1 : stoneColor2;
                float stoneLength = tileW - gapY;
                drawBrick(QVector3D(m_width / 2 - tileD / 2 - 0.01f, yPos, zPos), tileD - gap, tileH - gap, stoneLength, stoneColor);
            }
        }
    }
    
    
    // Mur du fond (z = -m_length)
    glPushMatrix();
    glTranslatef(0, 0, -m_length);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, wallDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, wallAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wallSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, wallShininess);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(-m_width / 2, 0, 0);
    glVertex3f(m_width / 2, 0, 0);
    glVertex3f(m_width / 2, m_height, 0);
    glVertex3f(-m_width / 2, m_height, 0);
    glEnd();

    // --- Petites briques (lingots) en tas arrondi ---
    tileW = 0.45f, tileH = 0.15f, tileD = 0.9f;
    gap = 0.025f;
    float gapX = 0.08f;
    int colsLingots = static_cast<int>(m_width / tileW + 2);
    int rowsLingots = static_cast<int>(m_height / tileH + 1);

    // Paramètres du tas arrondi
    float pileCenterX = 0.0f;
    float pileRadius = m_width * 0.4f; // rayon du tas
    float pileMaxHeight = m_height * 0.6f; // hauteur max du tas (peut ajuster)
    float pileYOffset = 0.0f; // décalage vertical du tas

    for (int y = 0; y < rowsLingots; ++y) {
        float yPos = y * tileH + tileH / 2 + gap / 2;
        // Calcul de la largeur du tas à cette hauteur (demi-ellipse)
        float normY = yPos / pileMaxHeight;
        if (normY > 1.0f) continue; // au-dessus du tas
        float pileHalfWidth = pileRadius * std::sqrt(1.0f - normY * normY); // demi-ellipse
        int minX = static_cast<int>(std::ceil((-pileHalfWidth - (-m_width / 2)) / tileW)) - 1;
        int maxX = static_cast<int>(std::floor((pileHalfWidth - (-m_width / 2)) / tileW)) + 1;
        for (int x = minX; x < maxX; ++x) {
            float xPos = -m_width / 2 + x * tileW + tileW / 2 + gapX / 2;
            // Vérifie si ce lingot est dans la forme du tas
            float dx = xPos - pileCenterX;
            if ((dx * dx) / (pileRadius * pileRadius) + (yPos * yPos) / (pileMaxHeight * pileMaxHeight) > 1.0f)
                continue;
            float offset = (y % 2) ? 0.05f / 2 : -0.05f / 2;
            QColor stoneColor = ((x + y) % 2 == 0) ? stoneColor1 : stoneColor2;
            float stoneWidth = tileW - gapX;
            drawBrick(QVector3D(xPos + offset, yPos + pileYOffset, tileD / 2 + 0.01f), stoneWidth, tileH - gap, tileD - gap, stoneColor);
        }
    }

    // --- Fond uni au-dessus (gap=0, profondeur réduite) ---
    // On place le fond juste au-dessus du tas
    float fondY = pileMaxHeight;
    float fondH = m_height - fondY;
    float fondD = 0.025f;
    if (fondH > 0.01f) {
        glPushMatrix();
        glTranslatef(0, fondY + fondH / 2, tileD / 2 + 0.015f);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, wallDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, wallAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wallSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, wallShininess);
        glScalef(m_width, fondH, fondD);
        float s = 0.5f;
        glBegin(GL_QUADS);
        // Face avant
        glNormal3f(0, 0, 1);
        glVertex3f(-s, -s, s);
        glVertex3f(s, -s, s);
        glVertex3f(s, s, s);
        glVertex3f(-s, s, s);
        // Face arrière
        glNormal3f(0, 0, -1);
        glVertex3f(-s, -s, -s);
        glVertex3f(-s, s, -s);
        glVertex3f(s, s, -s);
        glVertex3f(s, -s, -s);
        // Face gauche
        glNormal3f(-1, 0, 0);
        glVertex3f(-s, -s, -s);
        glVertex3f(-s, -s, s);
        glVertex3f(-s, s, s);
        glVertex3f(-s, s, -s);
        // Face droite
        glNormal3f(1, 0, 0);
        glVertex3f(s, -s, -s);
        glVertex3f(s, s, -s);
        glVertex3f(s, s, s);
        glVertex3f(s, -s, s);
        // Face dessus
        glNormal3f(0, 1, 0);
        glVertex3f(-s, s, -s);
        glVertex3f(-s, s, s);
        glVertex3f(s, s, s);
        glVertex3f(s, s, -s);
        // Face dessous
        glNormal3f(0, -1, 0);
        glVertex3f(-s, -s, -s);
        glVertex3f(s, -s, -s);
        glVertex3f(s, -s, s);
        glVertex3f(-s, -s, s);
        glEnd();
        glPopMatrix();
    }

    glPopMatrix();
}

void Corridor::drawGround()
{
    // Sauvegarde la couleur courante
    GLfloat prevColor[4];
    glGetFloatv(GL_CURRENT_COLOR, prevColor);

    // Sol principal (plan simple, sombre)
    glPushMatrix();
    // Couleur sol : pierre chaude, désaturée
    GLfloat groundDiffuse[4] = {140.0f / 255.0f, 110.0f / 255.0f, 60.0f / 255.0f, 1.0f};
    GLfloat groundAmbient[4] = {0.4f * 140.0f / 255.0f, 0.4f * 110.0f / 255.0f, 0.4f * 60.0f / 255.0f, 1.0f};
    GLfloat groundSpecular[4] = {0.12f, 0.10f, 0.07f, 1.0f};
    GLfloat groundShininess = 8.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, groundShininess);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-m_width / 2, 0.0f, 0.0f);
    glVertex3f(m_width / 2, 0.0f, 0.0f);
    glVertex3f(m_width / 2, 0.0f, -m_length);
    glVertex3f(-m_width / 2, 0.0f, -m_length);
    glEnd();
    glPopMatrix();
    // Motif pavé : cubes plats
    float tileW = 0.6f, tileD = 0.9f, tileH = 0.04f;
    float gap = 0.025f;
    float gapZ = 0.08f;
    int cols = static_cast<int>(m_width / tileW + 1);
    int rows = static_cast<int>(m_length / tileD + 1);
    QColor stoneColor1(140, 110, 60); // ocre pierre
    QColor stoneColor2(170, 140, 80); // ocre clair
    for (int x = -1; x < cols; ++x) {
        for (int z = -1; z < rows; ++z) {
            float xPos = -m_width / 2 + x * tileW + tileW / 2 + gap/2;
            float zPos = -z * tileD - tileD / 2 - gapZ/2;
            float offset = (z % 2) ? tileW / 2 : 0;
            QColor stoneColor = ((x + z) % 2 == 0) ? stoneColor1 : stoneColor2;
            drawBrick(QVector3D(xPos + offset, tileH / 2 + 0.01f, zPos), tileW - gap, tileH, tileD - gapZ, stoneColor);
        }
    }

    // Restaure la couleur courante
    glColor4fv(prevColor);
}

void Corridor::drawRoof()
{
    // Sauvegarde la couleur courante
    GLfloat prevColor[4];
    glGetFloatv(GL_CURRENT_COLOR, prevColor);

    // Plafond principal (plan simple, sombre)
    glPushMatrix();
    // Couleur plafond : pierre chaude, désaturée
    GLfloat roofDiffuse[4] = {140.0f / 255.0f, 110.0f / 255.0f, 60.0f / 255.0f, 1.0f};
    GLfloat roofAmbient[4] = {0.4f * 140.0f / 255.0f, 0.4f * 110.0f / 255.0f, 0.4f * 60.0f / 255.0f, 1.0f};
    GLfloat roofSpecular[4] = {0.12f, 0.10f, 0.07f, 1.0f};
    GLfloat roofShininess = 8.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, roofDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, roofAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, roofSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, roofShininess);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0); // Normale vers le bas
    glVertex3f(-m_width / 2, m_height, 0.0f);
    glVertex3f(m_width / 2, m_height, 0.0f);
    glVertex3f(m_width / 2, m_height, -m_length);
    glVertex3f(-m_width / 2, m_height, -m_length);
    glEnd();
    glPopMatrix();
    // Motif pavé : cubes plats
    float tileW = 0.9f, tileD = 2.1f, tileH = 0.04f;
    float gap = 0.0f;
    float gapZ = 0.0f;
    int cols = static_cast<int>(m_width / tileW + 1);
    int rows = static_cast<int>(m_length / tileD + 1);
    QColor stoneColor1(140, 110, 60); // ocre pierre
    QColor stoneColor2(170, 140, 80); // ocre clair
    for (int x = -1; x < cols; ++x) {
        for (int z = -1; z < rows; ++z) {
            float xPos = -m_width / 2 + x * tileW + tileW / 2 + gap/2;
            float zPos = -z * tileD - tileD / 2 - gapZ/2;
            float offset = (z % 2) ? tileW / 2 : 0;
            QColor stoneColor = ((x + z) % 2 == 0) ? stoneColor1 : stoneColor2;
            // On place les tuiles au plafond, orientées vers le bas
            drawBrick(QVector3D(xPos + offset, m_height - tileH / 2 - 0.01f, zPos), tileW - gap, tileH, tileD - gapZ, stoneColor);
        }
    }

    // Restaure la couleur courante
    glColor4fv(prevColor);
}

void Corridor::drawTorches()
{
    // 3 torches de chaque côté, espacées régulièrement
    const int nbTorches = 3;
    float hauteur = 2.5f;
    float decalageX = m_width / 2 - 0.5f;
    float margeZ = 5.0f;
    float espaceZ = (m_length - 2 * margeZ) / (nbTorches - 1);

    // Temps courant en secondes (pour l'effet smooth)
    qint64 ms = QTime::currentTime().msecsSinceStartOfDay();
    float t = ms / 1000.0f;

    for (int i = 0; i < nbTorches; ++i) {
        float z = -margeZ - i * espaceZ;
        // Torche gauche
        QVector3D posL(-decalageX, hauteur, z);
        int lightIdxL = GL_LIGHT0 + i; // GL_LIGHT0, GL_LIGHT1, GL_LIGHT2
        drawTorch(posL, true, lightIdxL);
        // Torche droite
        QVector3D posR(decalageX, hauteur, z);
        int lightIdxR = GL_LIGHT3 + i; // GL_LIGHT3, GL_LIGHT4, GL_LIGHT5
        drawTorch(posR, false, lightIdxR);
    }
    // Torche centrale sur le mur du fond
    float yFond = 0.75f * m_height;
    QVector3D posFond(0.0f, yFond, -m_length + 0.5f); // 0.5f pour la décoller du mur
    drawTorch(posFond, false, GL_LIGHT6);
}

void Corridor::drawTorch(const QVector3D &pos, bool leftWall, int lightIdx)
{
    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    // Manche de la torche (cylindre)
    glColor3f(0.3f, 0.2f, 0.1f);
    GLUquadric *quad = gluNewQuadric();
    glRotatef(90, 1, 0, 0);
    gluCylinder(quad, 0.07, 0.06, 0.45, 8, 1);
    glRotatef(-90, 1, 0, 0);
    gluDeleteQuadric(quad);
    // Tête de la torche (cube low-poly)
    glTranslatef(0, 0.02, 0); // On monte la tête au bout du manche
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glScalef(0.13f, 0.13f, 0.13f);
    float s = 0.5f;
    glBegin(GL_QUADS);
    // Face avant
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, s);
    // Face arrière
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, -s, -s);
    // Face gauche
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, s, -s);
    // Face droite
    glVertex3f(s, -s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, s, s);
    glVertex3f(s, -s, s);
    // Face dessus
    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, s);
    glVertex3f(s, s, s);
    glVertex3f(s, s, -s);
    // Face dessous
    glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, -s, s);
    glVertex3f(-s, -s, s);
    glEnd();
    glPopMatrix();
    // Flamme
    drawFlame(QVector3D(0, 0.05f, 0), 0.11f, 0.28f);

    // Lumière positionnée exactement à la flamme
    float t = QTime::currentTime().msecsSinceStartOfDay() / 1000.0f;
    float flicker = 0.03f * std::sin(t * 1.0f + lightIdx * 1.7f)
        + 0.01f * std::sin(t * 30.0f + lightIdx * 2.3f)
        + 0.005f * std::sin(t * 27.0f + lightIdx * 0.9f);
    float lightPower = 0.7f + flicker;
    GLfloat diffuse[4] = {0.95f * lightPower, 0.65f * lightPower, 0.18f * lightPower, 1.0f};
    GLfloat ambient[4] = {0.18f * lightPower, 0.10f * lightPower, 0.03f * lightPower, 1.0f};
    GLfloat specular[4] = {0.5f * lightPower, 0.35f * lightPower, 0.08f * lightPower, 1.0f};
    
    float zOffset = (lightIdx == GL_LIGHT6) ? 0.25f : 0.0f;
    GLfloat posLarr[4] = {0.0f, 0.3f, zOffset, 1.0f}; // position locale de la flamme
    glEnable(lightIdx);
    glLightfv(lightIdx, GL_POSITION, posLarr);
    glLightfv(lightIdx, GL_DIFFUSE, diffuse);
    glLightfv(lightIdx, GL_AMBIENT, ambient);
    glLightfv(lightIdx, GL_SPECULAR, specular);
    glLightf(lightIdx, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(lightIdx, GL_LINEAR_ATTENUATION, 0.5f);
    glLightf(lightIdx, GL_QUADRATIC_ATTENUATION, 0.5f);

    glPopMatrix();
}

void Corridor::drawBrick(const QVector3D &pos, float w, float h, float d, const QColor &color)
{
    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    // Définir le matériau pour la lumière
    GLfloat diffuse[4] = {color.redF(), color.greenF(), color.blueF(), 1.0f};
    GLfloat ambient[4] = {color.redF() * 0.4f, color.greenF() * 0.4f, color.blueF() * 0.4f, 1.0f};
    GLfloat specular[4] = {0.18f, 0.16f, 0.13f, 1.0f};
    GLfloat shininess = 12.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glScalef(w, h, d);
    float s = 0.5f;
    glColor3f(1.0f, 1.0f, 1.0f); // Remet la couleur courante à blanc
    glBegin(GL_QUADS);
    // Face avant
    glNormal3f(0, 0, 1);
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, s);
    // Face arrière
    glNormal3f(0, 0, -1);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, -s, -s);
    // Face gauche
    glNormal3f(-1, 0, 0);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, s, -s);
    // Face droite
    glNormal3f(1, 0, 0);
    glVertex3f(s, -s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, s, s);
    glVertex3f(s, -s, s);
    // Face dessus
    glNormal3f(0, 1, 0);
    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, s);
    glVertex3f(s, s, s);
    glVertex3f(s, s, -s);
    // Face dessous
    glNormal3f(0, -1, 0);
    glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, -s, s);
    glVertex3f(-s, -s, s);
    glEnd();
    glPopMatrix();
}

void Corridor::drawStoneTile(const QVector3D &pos, float w, float d, const QColor &color)
{
    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    glColor3f(color.redF(), color.greenF(), color.blueF());
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-w/2, 0, -d/2);
    glVertex3f(w/2, 0, -d/2);
    glVertex3f(w/2, 0, d/2);
    glVertex3f(-w/2, 0, d/2);
    glEnd();
    glPopMatrix();
}

void Corridor::drawFlame(const QVector3D &pos, float base, float height)
{
    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.7f, 0.1f); // Jaune/orange
    glVertex3f(0, 0, 0);
    glVertex3f(-base/2, 0, base/2);
    glVertex3f(0, height, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(base/2, 0, base/2);
    glVertex3f(0, height, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, -base);
    glVertex3f(0, height, 0);
    glEnd();
    glPopMatrix();
}
