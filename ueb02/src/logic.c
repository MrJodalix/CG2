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
#include "texture.h"

/* ---- Globale Daten ---- */

static int numberOfCheckPoints = 16;
static int numberOfInterpolationPoints = 25;
static int actualCheckPoint = 0;
static int normalToggle = 0;
static int helpToggle = 0;
static int toggleCheckPoint = 1;
static int toggleInterPoint = 1;
static int texture = 0;
const CGColor3f HIGHLEVEL = {1.0f, 0.0f, 0.0f};
static const CGColor3f MIDLEVEL = {1.0f, 1.0f, 0.0f};
static const CGColor3f LOWLEVEL = {0.0f, 0.0f, 1.0f};
static const CGColor3f UNDERLEVEL = {0.2f, 0.2f, 0.2f};
static const CGPoint3f HEIGHTLEVEL = {-0.3f, 0.0f, 0.3f};

// Matrix fuer Spline-Kurven Interpolation ohne Skalar 1/6 (kommt bei der Berechnung wieder dazu)
static float splineMat[4][4] = {{-1.0f, 3.0f, -3.0f, 1.0f},
                                {3.0f, -6.0f, 3.0f, 0.0f},
                                {-3.0f, 0.0f, 3.0f, 0.0f},
                                {1.0f, 4.0f, 1.0f, 0.0f}};

// Geometriematrix (4x4 Kontrollpunkte) Beispiel
float geometryMat[4][4] = {{0.1f, 0.2f, 0.1f, 0.3f},
                           {0.0f, 0.2f, 0.6f, 0.0f},
                           {0.3f, 0.5f, 0.1f, 0.3f},
                           {0.1f, 0.2f, -0.1f, 0.0f}};

// Hilfsmatrix fuer die Splineflaechen-Berechnung
float helpMat[4][4] = {{0.0f, 0.0f, 0.0f, 0.0f},
                       {0.0f, 0.0f, 0.0f, 0.0f},
                       {0.0f, 0.0f, 0.0f, 0.0f},
                       {0.0f, 0.0f, 0.0f, 0.0f}};

// Arrays für Vertexarray anlegen
Vertex *Vertices;
Normal *Normals;
GLuint *Indices;

/* ---- Funktionen ---- */

/**
 * Berechnet die Normalen der Kurve und fuegt sie dem Normalen-Array hinzu.
 * @param count der Index im Array
   @param qs die Steigung, wenn man nach s ableitet
   @param qt die Steigung, wenn man nach t ableitet
 */
void calcNormals(int count, float qs, float qt)
{
  CGPoint3f sVec = {0.0f, 0.0f, 0.0f};
  CGPoint3f tVec = {0.0f, 0.0f, 0.0f};
  CGPoint3f crossVec = {0.0f, 0.0f, 0.0f};
  double length = 0.0;

  // Tangenten berechnen
  tVec[0] = 1;
  tVec[1] = qt;
  tVec[2] = 0;

  sVec[0] = 0;
  sVec[1] = qs;
  sVec[2] = 1;

  // Kreuzprodukt zwischen den Tangenten
  crossVec[0] = sVec[1] * tVec[2] - sVec[2] * tVec[1];
  crossVec[1] = sVec[2] * tVec[0] - sVec[0] * tVec[2];
  crossVec[2] = sVec[0] * tVec[1] - sVec[1] * tVec[0];

  // Normierung
  length = sqrt(pow(crossVec[0], 2.0) + pow(crossVec[1], 2.0) + pow(crossVec[2], 2.0));

  Normals[count][NX] = (crossVec[0] / length);
  Normals[count][NY] = (crossVec[1] / length);
  Normals[count][NZ] = (crossVec[2] / length);
}

/**
 * Setzt die Koordinaten des Vertex-Arrays.
 * @param count  der Index im Array
 * @param t t der Koordinate
 * @param s s der Koordinate
 * @param coordX die berechnete X-Koordinate
 * @param coordY die berechnete Y-Koordinate
 * @param coordZ die berechnete Y-Koordinate
 * @param color die Farbe des Vertices
 */
void setVerticeCoords(int count, double t, double s, double coordX, double coordY, double coordZ, CGColor3f color)
{
  Vertices[count][CX] = coordX;
  Vertices[count][CY] = coordY;
  Vertices[count][CZ] = coordZ;
  Vertices[count][CR] = color[0];
  Vertices[count][CG] = color[1];
  Vertices[count][CB] = color[2];
  Vertices[count][CS] = s;
  Vertices[count][CT] = t;
}

/**
 * Reserviert den Speicher für das Vertex-Array.
 */
void initVertices(void)
{
  int *numberOfInterPoints = getNumberOfInterPoints();
  Vertices = malloc(sizeof(Vertex) * (*numberOfInterPoints));
  Indices = malloc(sizeof(Indices) * COUNT_OF_INDICES);
  Normals = malloc(sizeof(Normal) * (*numberOfInterPoints + 1) * 3);
}

/**
 * Leert den Speicher fuer das Vertex-Array.
 */
void freeAll(void)
{
  free(Vertices);
  Vertices = NULL;
  free(Indices);
  Indices = NULL;
  free(Normals);
  Normals = NULL;
}

/**
 * Berechnung der Spline-Flaeche.
 */
void calcSplinePlane(void)
{
  double coordX = XSTARTINTPOINT, coordY = 0.0, coordZ = ZSTARTINTPOINT, s = 0.0, t = 0.0;
  int i = 0, j = 0, col = 0, row = 0, count = 0;
  float sum = 0.0;
  initVertices();

  // Hilfsmatrix befuellen (Multiplikation der Spline-Matrix mit der Geometriematrix)
  for (row = 0; row < 4; ++row)
  {
    for (col = 0; col < 4; ++col)
    {
      sum = 0.0;
      for (i = 0; i < 4; ++i)
      {
        sum += splineMat[row][i] * geometryMat[i][col];
      }
      helpMat[row][col] = sum;
    }
  }

  // Schleife ueber alle interpolations Punkte
  for (j = 0; j < numberOfInterpolationPoints; j += sqrt(numberOfInterpolationPoints))
  {
    s = ((double)(j / sqrt(numberOfInterpolationPoints)) / (double)(numberOfInterpolationPoints - 1) * (sqrt(numberOfInterpolationPoints) + 1));

    for (i = 0; i < sqrt(numberOfInterpolationPoints); i++)
    {
      t = ((double)(i) / (double)(numberOfInterpolationPoints - 1) * (sqrt(numberOfInterpolationPoints) + 1));

      float s1 = (pow(s, 3.0) * helpMat[0][0] + pow(s, 2.0) * helpMat[1][0] + s * helpMat[2][0] + helpMat[3][0]);
      float s2 = (pow(s, 3.0) * helpMat[0][1] + pow(s, 2.0) * helpMat[1][1] + s * helpMat[2][1] + helpMat[3][1]);
      float s3 = (pow(s, 3.0) * helpMat[0][2] + pow(s, 2.0) * helpMat[1][2] + s * helpMat[2][2] + helpMat[3][2]);
      float s4 = (pow(s, 3.0) * helpMat[0][3] + pow(s, 2.0) * helpMat[1][3] + s * helpMat[2][3] + helpMat[3][3]);

      float t1 = (splineMat[0][0] * pow(t, 3.0) + splineMat[1][0] * pow(t, 2.0) + splineMat[2][0] * t + splineMat[3][0]);
      float t2 = (splineMat[0][1] * pow(t, 3.0) + splineMat[1][1] * pow(t, 2.0) + splineMat[2][1] * t + splineMat[3][1]);
      float t3 = (splineMat[0][2] * pow(t, 3.0) + splineMat[1][2] * pow(t, 2.0) + splineMat[2][2] * t + splineMat[3][2]);
      float t4 = (splineMat[0][3] * pow(t, 3.0) + splineMat[1][3] * pow(t, 2.0) + splineMat[2][3] * t + splineMat[3][3]);

      float sn1 = (3 * pow(s, 2.0) * helpMat[0][0] + 2 * s * helpMat[1][0] + helpMat[2][0]);
      float sn2 = (3 * pow(s, 2.0) * helpMat[0][1] + 2 * s * helpMat[1][1] + helpMat[2][1]);
      float sn3 = (3 * pow(s, 2.0) * helpMat[0][2] + 2 * s * helpMat[1][2] + helpMat[2][2]);
      float sn4 = (3 * pow(s, 2.0) * helpMat[0][3] + 2 * s * helpMat[1][3] + helpMat[2][3]);

      float tn1 = (splineMat[0][0] * 3 * pow(t, 2.0) + splineMat[1][0] * 2 * t + splineMat[2][0]);
      float tn2 = (splineMat[0][1] * 3 * pow(t, 2.0) + splineMat[1][1] * 2 * t + splineMat[2][1]);
      float tn3 = (splineMat[0][2] * 3 * pow(t, 2.0) + splineMat[1][2] * 2 * t + splineMat[2][2]);
      float tn4 = (splineMat[0][3] * 3 * pow(t, 2.0) + splineMat[1][3] * 2 * t + splineMat[2][3]);

      coordY = (1.0 / 36.0) * ((s1 * t1) + (s2 * t2) + (s3 * t3) + (s4 * t4));

      float qs = (1.0 / 36.0) * ((sn1 * t1) + (sn2 * t2) + (sn3 * t3) + (sn4 * t4));
      float qt = (1.0 / 36.0) * ((s1 * tn1) + (s2 * tn2) + (s3 * tn3) + (s4 * tn4));

      int state = !getTexturingStatus();
      CGColor3f color = {1.0, 1.0, 1.0};
      if (state)
      {
        if (coordY >= HEIGHTLEVEL[2])
        {
          color[0] = HIGHLEVEL[0];
          color[1] = HIGHLEVEL[1];
          color[2] = HIGHLEVEL[2];
        }
        if (coordY < HEIGHTLEVEL[2])
        {
          color[0] = MIDLEVEL[0];
          color[1] = MIDLEVEL[1];
          color[2] = MIDLEVEL[2];
        }
        if (coordY < HEIGHTLEVEL[1])
        {
          color[0] = LOWLEVEL[0];
          color[1] = LOWLEVEL[1];
          color[2] = LOWLEVEL[2];
        }
        if (coordY < HEIGHTLEVEL[0])
        {
          color[0] = UNDERLEVEL[0];
          color[1] = UNDERLEVEL[1];
          color[2] = UNDERLEVEL[2];
        }
      }

      setVerticeCoords(i + j, t, s, coordX, coordY, coordZ, color);
      calcNormals(i + j, qs, qt);

      coordX += (((-XSTARTINTPOINT) + XENDINTPOINT) / (sqrt(numberOfInterpolationPoints) - 1.0));
    }
    coordZ += (((-ZSTARTINTPOINT) + ZENDINTPOINT) / (sqrt(numberOfInterpolationPoints) - 1.0));
    coordX = XSTARTINTPOINT;
  }

  count = 0;
  for (j = 0; j < (numberOfInterpolationPoints)-sqrt(numberOfInterpolationPoints); j += sqrt((numberOfInterpolationPoints)))
  {
    for (i = 0; i < (sqrt((numberOfInterpolationPoints)) - 1); i++)
    {
      {
        Indices[count++] = i + j;
        Indices[count++] = j + sqrt((numberOfInterpolationPoints)) + i;
        Indices[count++] = i + j + 1;
        Indices[count++] = i + j + 1;
        Indices[count++] = j + sqrt((numberOfInterpolationPoints)) + i;
        Indices[count++] = j + sqrt((numberOfInterpolationPoints)) + i + 1;
      }
    }
  }
}

/*
 * Erhöht die Anzahl der Vertices im Vertex-Array.
 */
void addVertices()
{
  numberOfInterpolationPoints = pow(sqrt(numberOfInterpolationPoints) + 1, 2.0);
}

/*
 * Verringert die Anzahl der Vertices im Vertex-Array.
 */
void deleteVertices()
{
  if (numberOfInterpolationPoints > 4)
  {
    numberOfInterpolationPoints = pow(sqrt(numberOfInterpolationPoints) - 1, 2.0);
  }
}

/**
 * Wechselt den aktuellen Checkpoint.
 *
 * @param leftOrRight 0 ist links / != 0 ist rechts
 */
void switchCheckPoint(int leftOrRight)
{
  if (leftOrRight)
  {
    actualCheckPoint = (actualCheckPoint + 1) % numberOfCheckPoints;
  }
  else
  {
    actualCheckPoint--;
    if (actualCheckPoint < 0)
    {
      actualCheckPoint = numberOfCheckPoints - 1;
    }
  }
}

/**
 * Aendert die Hoehe fuer die Kontrollpunkte.
 *
 * @param upOrDown 0 ist oben / != 0 ist unten
 */
void switchHeight(int upOrDown)
{
  if (upOrDown)
  {
    geometryMat[actualCheckPoint / 4][actualCheckPoint % 4] = geometryMat[actualCheckPoint / 4][actualCheckPoint % 4] - 0.1f;
  }
  else
  {
    geometryMat[actualCheckPoint / 4][actualCheckPoint % 4] = geometryMat[actualCheckPoint / 4][actualCheckPoint % 4] + 0.1f;
  }
  freeAll();
  calcSplinePlane();
}

/**
 * Aendert den Toggle fuer Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void toggleHelp(void)
{
  helpToggle = (helpToggle + 1) % 2;
}

/*
 * Gibt den Status fuer die Hilfeausgabe zurueck.
 */
int *getHelp(void)
{
  return &helpToggle;
}

/**
 * Andert den Toggle fuer die Kontrollpunkteanzeige.
 */
void checkPointToggle(void)
{
  toggleCheckPoint = (toggleCheckPoint + 1) % 2;
}

/**
 * Getter fuer den Toggle fuer die Kontrollpunkteanzeige.
 */
int *getCheckPoint(void)
{
  return &toggleCheckPoint;
}

/**
 * Aendert den Toggle fuer die interpolierten Punkte.
 */
void interpolatePointToggle(void)
{
  toggleInterPoint = (toggleInterPoint + 1) % 2;
}

/**
 * Getter fuer den Toggle fuer die interpolierten Punkte.
 */
int *getInterpolationPoint(void)
{
  return &toggleInterPoint;
}

/*
 * Aendert den Switch fuer die Textur.
 */
void switchTexture(void)
{
  texture = (texture + 1) % 4;
}

/*
 * Gibt den Switch fuer die Textur zurueck.
 */
int *getSwitchTexture(void)
{
  return &texture;}

/*
 * Gibt den Status fuer die Normalen zurueck.
 */
int *getNormalsToggle(void)
{
  return &normalToggle;
}

/**
 * Gibt die Anzahl aller Vertices zurueck.
 */
int *getNumberOfCheckPoints(void)
{
  return &numberOfCheckPoints;
}

/**
 * Gibt die Anzahl aller Vertices zurueck.
 */
int *getNumberOfInterPoints(void)
{
  return &numberOfInterpolationPoints;
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
 * Liefert das Normals-Array zurueck.
 * @return Das Normals-Array.
 */
Normal *
getNormals(void)
{
  return Normals;
}

/**
 * Liefert den ersten Wert der Geometrie Matrix.
 */
float *
getGeometryMat(void)
{
  return *geometryMat;
}

/**
 * @return Liefert den aktuellen Kontrollpunkt, der veraendert werden kann.
 */
int *getActualCheckPoint(void)
{
  return &actualCheckPoint;
}