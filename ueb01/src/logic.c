/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Position und Bewegung der Objekte im Spiel.
 * Die Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * @author Joshua-Scott Schoettke, Ilana Schmara (Gruppe 1).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"

/* ---- Globale Daten ---- */
/** Mittelpunkt der Rakete */
static const float PI = 3.1415926;
static CGPoint2f g_rocketCenter = {0.0f, 0.0f};
static CGPoint2f g_checkpointStart = {BORDER_LEFT, 0.0f};
static CGPoint2f g_checkpointEnd = {BORDER_RIGHT, 0.0f};
static int g_currentLevel = 3;
static int numberOfVertices = 7;
static int normalToggle = 0;
static int helpToggle = 0;
static int hullToggle = 0;
static int pause = 0;
static int countOfHullIndices = 0;
static int bezierToggle = 0;

static float splineMat[4][4] = {{-1.0f * (1.0f / 6.0f), 3.0f * (1.0f / 6.0f), -3.0f * (1.0f / 6.0f), 1.0f * (1.0f / 6.0f)},
                                {3.0f * (1.0f / 6.0f), -6.0f * (1.0f / 6.0f), 3.0f * (1.0f / 6.0f), 0.0f},
                                {-3.0f * (1.0f / 6.0f), 0.0f, 3.0f * (1.0f / 6.0f), 0.0f},
                                {1.0f * (1.0f / 6.0f), 4.0f * (1.0f / 6.0f), 1.0f * (1.0f / 6.0f), 0.0f}};

static float bezierMat[4][4] = {{-1.0f, 3.0f, -3.0f, 1.0f},
                                {3.0f, -6.0f, 3.0f, 0.0f},
                                {-3.0f, 3.0f, 0.0f, 0.0f},
                                {1.0f, 0.0f, 0.0f, 0.0f}};

CGPoint2f g_checkpoint1 = {-0.5f, 0.5f};
CGPoint2f g_checkpoint2 = {-0.3f, -0.5f};
CGPoint2f g_checkpoint3 = {0.3f, 0.3f};
CGPoint2f g_checkpoint4 = {0.7f, 0.4f};

// Arrays für Vertexarray anlegen
Vertex *Vertices;
Normal *Normals;
GLuint *Indices;
GLuint *HullIndices;

/* ---- Funktionen ---- */

/**
 * Berechnet die Normalen der Kurve und fuegt sie dem Normalen-Array hinzu.
 * @param count der Index im Array
 */
void calcNormals(int count)
{
  double xLeft = 0.0,
         xRight = 0.0,
         yLeft = 0.0,
         yRight = 0.0,
         xVec1 = 0.0,
         yVec1 = 0.0,
         zVec1 = 0.0,
         xVecZ = 0.0,
         yVecZ = 0.0,
         zVecZ = 1.0,
         xVecCross = 0.0,
         yVecCross = 0.0,
         zVecCross = 0.0,
         length = 0.0;

  if (count == 0)
  {
    xLeft = Vertices[count][CX];
    yLeft = Vertices[count][CY];
  }
  else
  {
    xLeft = Vertices[count - 1][CX];
    yLeft = Vertices[count - 1][CY];
  }

  if (count == (numberOfVertices - 1))
  {
    xRight = Vertices[count][CX];
    yRight = Vertices[count][CY];
  }
  else
  {
    xRight = Vertices[count + 1][CX];
    yRight = Vertices[count + 1][CY];
  }
  // Verbindungsvektor
  xVec1 = xRight - xLeft;
  yVec1 = yRight - yLeft;
  // Kreuzprodukt zwischen Verbindungsvektor und Z-Vektor
  xVecCross = yVec1 * zVecZ - zVec1 * yVecZ;
  yVecCross = (zVec1 * xVecZ - xVec1 * zVecZ) * -1.0;
  zVecCross = xVec1 * yVecZ - yVec1 * xVecZ;
  // Normierung
  length = sqrt(pow(xVecCross, 2.0) + pow(yVecCross, 2.0) + pow(zVecCross, 2.0));
  Normals[count][NX] = (xVecCross) * (1.0 / length);
  Normals[count][NY] = (yVecCross) * (1.0 / length);
  Normals[count][NZ] = (zVecCross) * (1.0 / length);
}

/**
 * Setzt die Koordinaten des Vertex-Arrays.
 * @param count  der Index im Array
 * @param coordX die berechnete X-Koordinate
 * @param coordY die berechnete Y-Koordinate
 */
void setVerticeCoords(int count, double time, double coordX, double coordY)
{
  Vertices[count][T] = time;
  Vertices[count][CX] = coordX;
  Vertices[count][CY] = coordY;
  calcNormals(count);
}

/**
 * Setzt die Huellen-Koordinaten
 * @param count  der Index im Array
 * @param coordX die berechnete X-Koordinate der Huelle
 * @param coordY die berechnete Y-Koordinate der Huelle
 */
void setHullCords(int count, double coordX, double coordY)
{
  Vertices[count][HX] = coordX;
  Vertices[count][HY] = coordY;
}

/**
 * Initialisiert das Indices-Array.
 */
void initIndices(void)
{
  int idx = 0, count = 0;

  for (count = 0; count < numberOfVertices; count++)
  {
    Indices[idx++] = count;
  }
}

/**
 * Initialisiert das Indices-Array fuer die Huelle.
 */
void initHullIndices(int count)
{
  HullIndices[count] = count;
  countOfHullIndices = count;
}

/**
 *
 */
void calcSpline()
{
  double coordX = 0.0, coordY = 0.0, t = 0.0;
  int count = 0;

  switch (g_currentLevel)
  {
  case 1:
  {
    for (count = 0; count < numberOfVertices; count++)
    {
      t = ((double)(count) / (double)(numberOfVertices - 1));

      coordX =
          ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpointStart[0]) +
          ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint1[0]) +
          ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint2[0]) +
          ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpointEnd[0]);

      coordY =
          ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpointStart[1]) +
          ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint1[1]) +
          ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint2[1]) +
          ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpointEnd[1]);

      setVerticeCoords(count, t, coordX, coordY);
    }
  }
  break;
  case 2:
  {
    for (count = 0; count < numberOfVertices; count++)
    {
      if (((double)(count) < (double)(numberOfVertices - 1) / 2.0))
      {
        t = ((double)(count) / (double)(numberOfVertices - 1) * 2.0);
        if (t < 1.0)
        {
          coordX =
              ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpointStart[0]) +
              ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint1[0]) +
              ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint2[0]) +
              ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpoint3[0]);

          coordY =
              ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpointStart[1]) +
              ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint1[1]) +
              ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint2[1]) +
              ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpoint3[1]);

          setVerticeCoords(count, t, coordX, coordY);
        }
      }
      else
      {
        t = (((double)(count) / (double)(numberOfVertices - 1) * 2.0) - 1.0);
        coordX =
            ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpoint1[0]) +
            ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint2[0]) +
            ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint3[0]) +
            ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpointEnd[0]);

        coordY =
            ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpoint1[1]) +
            ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint2[1]) +
            ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint3[1]) +
            ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpointEnd[1]);

        setVerticeCoords(count, t, coordX, coordY);
      }
    }
  }
  break;
  case 3:
  {
    for (count = 0; count < numberOfVertices; count++)
    {
      if ((double)(count) < ((double)(numberOfVertices - 1) / 3.0))
      {
        t = ((double)(count) / (double)(numberOfVertices - 1) * 3.0);
        if (t < 1.0)
        {
          coordX =
              ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpointStart[0]) +
              ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint1[0]) +
              ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint2[0]) +
              ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpoint3[0]);

          coordY =
              ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpointStart[1]) +
              ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint1[1]) +
              ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint2[1]) +
              ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpoint3[1]);

          setVerticeCoords(count, t, coordX, coordY);
        }
      }
      else if ((double)(count) < ((double)(numberOfVertices - 1) / 3.0) * 2.0)
      {
        t = ((double)(count) / (double)(numberOfVertices - 1) * 3.0) - 1.0;
        if (t < 1.0)
        {
          coordX =
              ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpoint1[0]) +
              ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint2[0]) +
              ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint3[0]) +
              ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpoint4[0]);

          coordY =
              ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpoint1[1]) +
              ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint2[1]) +
              ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint3[1]) +
              ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpoint4[1]);

          setVerticeCoords(count, t, coordX, coordY);
        }
      }
      else
      {
        t = (((double)(count) / (double)(numberOfVertices - 1) * 3.0) - 2.0);
        coordX =
            ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpoint2[0]) +
            ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint3[0]) +
            ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint4[0]) +
            ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpointEnd[0]);

        coordY =
            ((pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]) * g_checkpoint2[1]) +
            ((pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]) * g_checkpoint3[1]) +
            ((pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]) * g_checkpoint4[1]) +
            ((pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]) * g_checkpointEnd[1]);

        setVerticeCoords(count, t, coordX, coordY);
      }
    }
  }
  break;
  default:
    break;
  }
}

/**
 * Berechnet die Koordinaten fuer die Bezier-Kurve und fuegt diese in das Vertex-Array hinzu.
 */
void calcBezier()
{
  double coordX = 0.0, coordY = 0.0, t = 0.0;
  int count = 0;

  for (count = 0; count < numberOfVertices; count++)
  {
    t = ((double)(count) / (double)(numberOfVertices - 1));

    if (t == 0.0)
    {
      coordX = g_checkpointStart[0];
      coordY = g_checkpointStart[1];
      setVerticeCoords(count, t, coordX, coordY);
    }
    else if ((t > 0.0) && (t < 1.0))
    {
      coordX =
          ((pow(t, 3.0) * bezierMat[0][0] + pow(t, 2.0) * bezierMat[1][0] + t * bezierMat[2][0] + bezierMat[3][0]) * g_checkpointStart[0]) +
          ((pow(t, 3.0) * bezierMat[0][1] + pow(t, 2.0) * bezierMat[1][1] + t * bezierMat[2][1] + bezierMat[3][1]) * g_checkpoint1[0]) +
          ((pow(t, 3.0) * bezierMat[0][2] + pow(t, 2.0) * bezierMat[1][2] + t * bezierMat[2][2] + bezierMat[3][2]) * g_checkpoint2[0]) +
          ((pow(t, 3.0) * bezierMat[0][3] + pow(t, 2.0) * bezierMat[1][3] + t * bezierMat[2][3] + bezierMat[3][3]) * g_checkpointEnd[0]);

      coordY =
          ((pow(t, 3.0) * bezierMat[0][0] + pow(t, 2.0) * bezierMat[1][0] + t * bezierMat[2][0] + bezierMat[3][0]) * g_checkpointStart[1]) +
          ((pow(t, 3.0) * bezierMat[0][1] + pow(t, 2.0) * bezierMat[1][1] + t * bezierMat[2][1] + bezierMat[3][1]) * g_checkpoint1[1]) +
          ((pow(t, 3.0) * bezierMat[0][2] + pow(t, 2.0) * bezierMat[1][2] + t * bezierMat[2][2] + bezierMat[3][2]) * g_checkpoint2[1]) +
          ((pow(t, 3.0) * bezierMat[0][3] + pow(t, 2.0) * bezierMat[1][3] + t * bezierMat[2][3] + bezierMat[3][3]) * g_checkpointEnd[1]);

      setVerticeCoords(count, t, coordX, coordY);
    }
    else if (t == 1.0)
    {
      coordX = g_checkpointEnd[0];
      coordY = g_checkpointEnd[1];
      setVerticeCoords(count, t, coordX, coordY);
    }
  }
}

/**
 * Berechnet die Positionen der einzelnen Kontrollpunkte, wenn sie mit der Maus bewegt werden.
 * @param mouseX X-Koordinate des Mauszeigers
 * @param mouseY Y-Koordinate des Mauszeigers
 */
void calcCheckpointPosition(float mouseX, float mouseY)
{
  float xPos = mouseX + CHECKPOINT_RADIUS;
  float xNeg = mouseX - CHECKPOINT_RADIUS;
  float yPos = mouseY + CHECKPOINT_RADIUS;
  float yNeg = mouseY - CHECKPOINT_RADIUS;

  if ((g_currentLevel > 2) && (g_checkpoint4[0] <= xPos) && (g_checkpoint4[0] >= xNeg) && (g_checkpoint4[1] <= yPos) && (g_checkpoint4[1] >= yNeg))
  {
    g_checkpoint4[0] = mouseX;
    g_checkpoint4[1] = mouseY;
  }
  else if ((g_currentLevel > 1) && (g_checkpoint3[0] <= xPos) && (g_checkpoint3[0] >= xNeg) && (g_checkpoint3[1] <= yPos) && (g_checkpoint3[1] >= yNeg))
  {
    g_checkpoint3[0] = mouseX;
    g_checkpoint3[1] = mouseY;
  }
  else if ((g_checkpoint2[0] <= xPos) && (g_checkpoint2[0] >= xNeg) && (g_checkpoint2[1] <= yPos) && (g_checkpoint2[1] >= yNeg))
  {
    g_checkpoint2[0] = mouseX;
    g_checkpoint2[1] = mouseY;
  }
  else if ((g_checkpoint1[0] <= xPos) && (g_checkpoint1[0] >= xNeg) && (g_checkpoint1[1] <= yPos) && (g_checkpoint1[1] >= yNeg))
  {
    g_checkpoint1[0] = mouseX;
    g_checkpoint1[1] = mouseY;
  }
  if (bezierToggle == 0)
  {
    calcSpline();
  }
  else
  {
    calcBezier();
  }
  initRocketPosition();
  calcConvexHull();
}

/**
 * Rechnet die Maus-Koordinaten in Bildschirmkoordinaten um.
 * @param x X-Koordinate des Mauszeigers
 * @param y Y-Koordinate des Mauszeigers
 * @param width Breite des Anzeigefensters
 * @param height Hoehe des Anzeigefensters
 */
void calcMousePosition(int x, int y, float width, float height)
{
  float mouseX = 0.0f, mouseY = 0.0f;
  float minimum = (float)fmin(width, height);
  float maximum = (float)fmax(width, height);
  mouseX = ((float)x / (minimum)-0.5f) * 2.0f;
  mouseY = ((float)y / (minimum)-0.5f) * 2.0f * -1.0f;
  if (width / height > 1)
  {
    mouseX = ((float)(x - (maximum - minimum) / 2) / (minimum)-0.5f) * 2.0f;
  }
  else
  {
    mouseY = ((float)(y - (maximum - minimum) / 2) / (minimum)-0.5f) * 2.0f * -1.0f;
  }
  if (mouseX >= 1.0f)
  {
    mouseX = 1.0f;
  }
  else if (mouseX <= -1.0f)
  {
    mouseX = -1.0f;
  }
  if (mouseY >= 1.0f)
  {
    mouseY = 1.0f;
  }
  else if (mouseY <= -1.0f)
  {
    mouseY = -1.0f;
  }
  calcCheckpointPosition(mouseX, mouseY);
}

/**
 * Setzt die Rakete auf ihre Startposition (zurueck).
 */
void initRocketPosition(void)
{
  g_rocketCenter[0] = Vertices[0][CX];
  g_rocketCenter[1] = Vertices[0][CY];
}

/**
 * Berechnet neue Position der Rakete.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void calcRocketPosition(double interval)
{
  if (pause == 0)
  {
  }
}

/**
 * Reserviert den Speicher für das Vertex-Array.
 */
void initVertices(void)
{
  int *numberOfVertices = getNumberOfVertices();
  Vertices = realloc(Vertices, sizeof(Vertex) * (*numberOfVertices));
  Indices = realloc(Indices, sizeof(GLuint) * COUNT_OF_INDICES);
  HullIndices = realloc(HullIndices, sizeof(GLuint) * COUNT_OF_INDICES);
  Normals = realloc(Normals, sizeof(Normal) * (*numberOfVertices));
}

/**
 * Leert den Speicher für das Vertex-Array.
 */
void freeAll(void)
{
  free(Vertices);
  Vertices = NULL;
  free(Indices);
  Indices = NULL;
  free(HullIndices);
  HullIndices = NULL;
  free(Normals);
  Normals = NULL;
}

/**
 * Berechnet die Konvexe Huelle.
 */
void calcConvexHull()
{
  float minY = 2.0f;
  int minX = -1;

  float unCalculatedList[6][2] = {{g_checkpointStart[0], g_checkpointStart[1]},
                                  {g_checkpoint1[0], g_checkpoint1[1]},
                                  {g_checkpoint2[0], g_checkpoint2[1]},
                                  {g_checkpointEnd[0], g_checkpointEnd[1]},
                                  {EMPTY_FIELD, EMPTY_FIELD},
                                  {EMPTY_FIELD, EMPTY_FIELD}};

  float calculatedList[7][2] = {{EMPTY_FIELD, EMPTY_FIELD},
                                {EMPTY_FIELD, EMPTY_FIELD},
                                {EMPTY_FIELD, EMPTY_FIELD},
                                {EMPTY_FIELD, EMPTY_FIELD},
                                {EMPTY_FIELD, EMPTY_FIELD},
                                {EMPTY_FIELD, EMPTY_FIELD},
                                {EMPTY_FIELD, EMPTY_FIELD}};
  if (g_currentLevel >= 2)
  {
    unCalculatedList[3][0] = g_checkpoint3[0];
    unCalculatedList[3][1] = g_checkpoint3[1];
    unCalculatedList[4][0] = g_checkpointEnd[0];
    unCalculatedList[4][1] = g_checkpointEnd[1];
  }
  if (g_currentLevel == 3)
  {
    unCalculatedList[4][0] = g_checkpoint4[0];
    unCalculatedList[4][1] = g_checkpoint4[1];
    unCalculatedList[5][0] = g_checkpointEnd[0];
    unCalculatedList[5][1] = g_checkpointEnd[1];
  }
  // Startpunktberechnung
  for (int i = 0; i < g_currentLevel + 3; i++)
  {
    if ((unCalculatedList)[i][1] < minY)
    {
      minY = (unCalculatedList)[i][1];
      minX = i;
    }
    else if ((unCalculatedList)[i][1] == minY)
    {
      if (minX >= 0 && (unCalculatedList)[i][0] < (unCalculatedList)[minX][0])
      {
        minY = (unCalculatedList)[i][1];
        minX = i;
      }
    }
  }
  calculatedList[0][0] = unCalculatedList[minX][0];
  calculatedList[0][1] = unCalculatedList[minX][1];

  CGPoint2f lastPoint = {EMPTY_FIELD, EMPTY_FIELD};
  int count = 0;
  float angle = 361.0f;
  float tempAngle = 361.0f;
  // float minAngle = 0.0f;

  while (calculatedList[0][0] != lastPoint[0] && calculatedList[0][0] != lastPoint[1])
  {
    lastPoint[0] = calculatedList[count][0];
    lastPoint[1] = calculatedList[count][1];

    for (int i = 0; i < g_currentLevel + 3; i++)
    {
      if (!((lastPoint[0] == unCalculatedList[i][0] && lastPoint[1] == unCalculatedList[i][1]) || (unCalculatedList[i][0] == EMPTY_FIELD && unCalculatedList[i][1] == EMPTY_FIELD)))
      {
        //Winkelberechnung von zwei Ortsvektoren
        angle = acosf((unCalculatedList[i][0] - lastPoint[0]) /
                      (sqrt(
                        pow(unCalculatedList[i][0] - lastPoint[0], 2) + pow(unCalculatedList[i][1] - lastPoint[1], 2) ))) *
                180.0f / PI;
        if (unCalculatedList[i][1] < lastPoint[1])
        {
          angle = 360.0f - angle;
        }
        if (tempAngle > angle)
        {
          tempAngle = angle;
          minX = i;
        }
      }
    }
    count++;
    lastPoint[0] = unCalculatedList[minX][0];
    lastPoint[1] = unCalculatedList[minX][1];
    calculatedList[count][0] = lastPoint[0];
    calculatedList[count][1] = lastPoint[1];
    unCalculatedList[minX][0] = EMPTY_FIELD;
    unCalculatedList[minX][1] = EMPTY_FIELD;
    tempAngle = 361.0f;
  }

  count = 0;
  while (count < sizeof(calculatedList)/sizeof(float)/2 && calculatedList[count][0] != EMPTY_FIELD && calculatedList[count][1] != EMPTY_FIELD)
  {
    setHullCords(count, calculatedList[count][0], calculatedList[count][1]);
    initHullIndices(count);
    count++;
  }
}

/*
 * Erhöht die Anzahl der Vertices im Vertex-Array.
 */
void addVertices()
{
  numberOfVertices++;
  initVertices();
  initIndices();
  if (bezierToggle == 0)
  {
    calcSpline();
  }
  else
  {
    calcBezier();
  }
}

/*
 * Verringert die Anzahl der Vertices im Vertex-Array.
 */
void deleteVertices()
{
  if (numberOfVertices > 2)
  {
    numberOfVertices--;
  }
  initVertices();
  initIndices();
  if (bezierToggle == 0)
  {
    calcSpline();
  }
  else
  {
    calcBezier();
  }
}

/**
 * Aendert den Wert für Pause auf den jeweils anderen Wert (0 oder 1).
 */
void togglePause(void)
{
  pause = (pause + 1) % 2;
}

/**
 * Aendert den Toggle für Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void toggleHelp(void)
{
  helpToggle = (helpToggle + 1) % 2;
}

/**
 * Aendert den Toggle für Hilfe auf den jeweils anderen Wert (0 oder 1)
 */
void toggleHull(void)
{
  hullToggle = (hullToggle + 1) % 2;
}

/**
 * Liefert aktuelle Postion (des Mittelpunktes) aller Waende.
 * @return Postion (des Mittelpunktes) der Waende.
 * Aendert den Toggle fuer die Bezier-Kurve auf den jeweils anderen Wert (0 oder 1).
 */
void toggleBezier(void)
{
  bezierToggle = (bezierToggle + 1) % 2;
  if (bezierToggle == 0)
  {
    calcSpline();
  }
  else
  {
    calcBezier();
  }
  initRocketPosition();
}

/**
 * Liefert aktuelle Postion (des Mittelpunktes) der Rakete.
 * @return Postion (des Mittelpunktes) der Rakete.
 */
CGPoint2f *
getRocketCenter(void)
{
  return &g_rocketCenter;
}

/**
 * Gibt den Status fuer Pause zurueck.
 */
int *getPause(void)
{
  return &pause;
}

/*
 * Gibt den Status fuer die Hilfeausgabe zurueck.
 */
int *getHelp(void)
{
  return &helpToggle;
}

/*
 * Gibt den Status fuer die Hilfeausgabe zurueck.
 */
int *getHull(void)
{
  return &hullToggle;
}

/*
 * Gibt den Status fuer die Normalen zurueck.
 */
int *getNormalsToggle(void)
{
  return &normalToggle;
}

/**
 * Gibt den Status fuer den Bezier-Toggle zurueck.
 */
int *getBezierToggle(void)
{
  return &bezierToggle;
}

/**
 * Liefert, den Index des aktuellen Levels.
 * @return den Index des aktuellen Levels.
 */
int *getCurrentLevel(void)
{
  return &g_currentLevel;
}

/**
 * Liefert aktuelle Postion des ersten Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpointStart(void)
{
  return &g_checkpointStart;
}

/**
 * Liefert aktuelle Postion des letzten Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpointEnd(void)
{
  return &g_checkpointEnd;
}

/**
 * Liefert aktuelle Postion des 2. Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpoint1(void)
{
  return &g_checkpoint1;
}

/**
 * Liefert aktuelle Postion des 3. Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpoint2(void)
{
  return &g_checkpoint2;
}

/**
 * Liefert aktuelle Postion des 4. Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpoint3(void)
{
  return &g_checkpoint3;
}

/**
 * Liefert aktuelle Postion des 5. Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpoint4(void)
{
  return &g_checkpoint4;
}

/**
 * Gibt die Anzahl aller Vertices zurueck.
 */
int *getNumberOfVertices(void)
{
  return &numberOfVertices;
}

/**
 * Gibt die Anzahl aller Vertices zurueck
 */
int *getCountOfVertices(void)
{
  return &countOfHullIndices;
}

/**
 * Liefert das Vertex-Array zurueck.
 * @return Das Vertex-Array.
 */
Vertex *
getVertices(void)
{
  return Vertices;
}

/**
 * Liefert das Indices-Array zurueck.
 * @return Das Indices-Array.
 */
GLuint *
getIndices(void)
{
  return Indices;
}

/**
 * Liefert das Indices-Array zurueck.
 * @return Das Indices-Array.
 */
GLuint *
getHullIndices(void)
{
  return HullIndices;
}

/**
 * Liefert das Normals-Array zurueck.
 * @return Das Normals-Array.
 */
Normal *
getNormals(void)
{
  return Normals;
}