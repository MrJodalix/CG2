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
static int numberOfInterpolationPoints = 625;
static int actualCheckPoint = 0;
static int normalToggle = 0;
static int helpToggle = 0;
static int toggleCheckPoint = 0;
static int toggleInterPoint = 0;
static int togglePause = 1;
static int selectWall = 0;
static int start = 0;
static int texture = 0;
const CGColor3f HIGHLEVEL = {1.0f, 0.0f, 0.0f};
static const CGColor3f MIDLEVEL = {1.0f, 1.0f, 0.0f};
static const CGColor3f LOWLEVEL = {0.0f, 0.0f, 1.0f};
static const CGColor3f UNDERLEVEL = {0.2f, 0.2f, 0.2f};
static const CGPoint3f HEIGHTLEVEL = {-0.3f, 0.0f, 0.3f};
CGPoint4f lightPos = {0.0f, 0.0f, 0.0f, 0.0f};
CGPoint8f ballPosition = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
int ballIndice = 0;

// Wandstartpunkte
static CGPoint3f wall0 = {-0.4f, 0.0f, 0.3f};
static CGPoint3f wall1 = {-0.2f, 0.0f, 0.1f};
static CGPoint3f wall2 = {0.0f, 0.0f, -0.3f};
static CGPoint3f wall3 = {0.2f, 0.0f, 0.0f};
static CGPoint3f wall4 = {0.1f, 0.0f, 0.34f};
static CGPoint3f wall5 = {0.0f, 0.0f, 0.3f};

// Matrix fuer Spline-Kurven Interpolation ohne Skalar 1/6 (kommt bei der Berechnung wieder dazu)
static float splineMat[4][4] = {{-1.0f, 3.0f, -3.0f, 1.0f},
                                {3.0f, -6.0f, 3.0f, 0.0f},
                                {-3.0f, 0.0f, 3.0f, 0.0f},
                                {1.0f, 4.0f, 1.0f, 0.0f}};

/** Geometriematrix (4x4 Kontrollpunkte) Beispiel */
float geometryMat[4][4] = {{0.4f, 0.6f, 0.4f, 0.4f},
                           {0.3f, 0.2f, 0.3f, 0.2f},
                           {-0.1f, 0.1f, -0.2f, 0.0f},
                           {0.2f, -0.2f, 0.0f, -0.1f}};

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
void calcNormals(CGPoint3f *calcedNormals, float qs, float qt)
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

  (*calcedNormals)[0] = (crossVec[0] / length);
  (*calcedNormals)[1] = (crossVec[1] / length);
  (*calcedNormals)[2] = (crossVec[2] / length);
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
  Vertices = malloc(sizeof(Vertex) * (numberOfInterpolationPoints));
  Indices = malloc(sizeof(Indices) * COUNT_OF_INDICES);
  Normals = malloc(sizeof(Normal) * (numberOfInterpolationPoints + 1) * 3);
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
 * Berechnet die Hilfsmatrix
 */
void calcHelpMat(void)
{
  float sum = 0.0;
  int i = 0, col = 0, row = 0;
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
}

/**
 * Berechnet die Indices
 * @param squert die Wurzel aus der Anzahl der Interpolationspunkte
 */
void calcIndices(double squert)
{
  int count = 0, i = 0, j = 0;
  for (j = 0; j < (numberOfInterpolationPoints)-squert; j += squert)
  {
    for (i = 0; i < (squert - 1); i++)
    {
      Indices[count++] = i + j;
      Indices[count++] = j + squert + i;
      Indices[count++] = i + j + 1;
      Indices[count++] = i + j + 1;
      Indices[count++] = j + squert + i;
      Indices[count++] = j + squert + i + 1;
    }
  }
}

/**
 * @brief berechnet die Werte fuer die normalen Berechnung
 * 
 * @param s die z Koordinate in [0-1]
 * @param t die x Koordinate in [0-1]
 * @param s1 Koordinateabschnitt
 * @param s2 Koordinateabschnitt
 * @param s3 Koordinateabschnitt
 * @param s4 Koordinateabschnitt
 * @param t1 Koordinateabschnitt
 * @param t2 Koordinateabschnitt
 * @param t3 Koordinateabschnitt
 * @param t4 Koordinateabschnitt
 * @return float die Y-Koordinate der Berechnung
 */
float calcSAndT(float s, float t, float *s1, float *s2, float *s3, float *s4, float *t1, float *t2, float *t3, float *t4)
{
  *s1 = (pow(s, 3.0) * helpMat[0][0] + pow(s, 2.0) * helpMat[1][0] + s * helpMat[2][0] + helpMat[3][0]);
  *s2 = (pow(s, 3.0) * helpMat[0][1] + pow(s, 2.0) * helpMat[1][1] + s * helpMat[2][1] + helpMat[3][1]);
  *s3 = (pow(s, 3.0) * helpMat[0][2] + pow(s, 2.0) * helpMat[1][2] + s * helpMat[2][2] + helpMat[3][2]);
  *s4 = (pow(s, 3.0) * helpMat[0][3] + pow(s, 2.0) * helpMat[1][3] + s * helpMat[2][3] + helpMat[3][3]);

  *t1 = (pow(t, 3.0) * splineMat[0][0] + pow(t, 2.0) * splineMat[1][0] + t * splineMat[2][0] + splineMat[3][0]);
  *t2 = (pow(t, 3.0) * splineMat[0][1] + pow(t, 2.0) * splineMat[1][1] + t * splineMat[2][1] + splineMat[3][1]);
  *t3 = (pow(t, 3.0) * splineMat[0][2] + pow(t, 2.0) * splineMat[1][2] + t * splineMat[2][2] + splineMat[3][2]);
  *t4 = (pow(t, 3.0) * splineMat[0][3] + pow(t, 2.0) * splineMat[1][3] + t * splineMat[2][3] + splineMat[3][3]);

  return (1.0f / 36.0f) * (((*s1) * (*t1)) + ((*s2) * (*t2)) + ((*s3) * (*t3)) + ((*s4) * (*t4)));
}

/**
 * @brief berechnet die Ableitungswerte fuer die normalen Berechnung
 * 
 * @param s die z Koordinate in [0-1]
 * @param t die x Koordinate in [0-1]
 * @param sn1 Koordinateabschnitt
 * @param sn2 Koordinateabschnitt
 * @param sn3 Koordinateabschnitt
 * @param sn4 Koordinateabschnitt
 * @param tn1 Koordinateabschnitt
 * @param tn2 Koordinateabschnitt
 * @param tn3 Koordinateabschnitt
 * @param tn4 Koordinateabschnitt
 */
void calcSnAndTn(float s, float t, float *sn1, float *sn2, float *sn3, float *sn4, float *tn1, float *tn2, float *tn3, float *tn4)
{
  *sn1 = (3 * pow(s, 2.0) * helpMat[0][0] + 2 * s * helpMat[1][0] + helpMat[2][0]);
  *sn2 = (3 * pow(s, 2.0) * helpMat[0][1] + 2 * s * helpMat[1][1] + helpMat[2][1]);
  *sn3 = (3 * pow(s, 2.0) * helpMat[0][2] + 2 * s * helpMat[1][2] + helpMat[2][2]);
  *sn4 = (3 * pow(s, 2.0) * helpMat[0][3] + 2 * s * helpMat[1][3] + helpMat[2][3]);

  *tn1 = (3 * pow(t, 2.0) * splineMat[0][0] + 2 * t * splineMat[1][0] + splineMat[2][0]);
  *tn2 = (3 * pow(t, 2.0) * splineMat[0][1] + 2 * t * splineMat[1][1] + splineMat[2][1]);
  *tn3 = (3 * pow(t, 2.0) * splineMat[0][2] + 2 * t * splineMat[1][2] + splineMat[2][2]);
  *tn4 = (3 * pow(t, 2.0) * splineMat[0][3] + 2 * t * splineMat[1][3] + splineMat[2][3]);
}

/**
 * Berechnung der Spline-Flaeche.
 */
void calcSplinePlane(void)
{
  double coordX = XSTARTINTPOINT, coordY = 0.0, coordZ = ZSTARTINTPOINT, s = 0.0, t = 0.0;
  int i = 0, j = 0;
  float s1 = 0.0f, s2 = 0.0f, s3 = 0.0f, s4 = 0.0f;
  float t1 = 0.0f, t2 = 0.0f, t3 = 0.0f, t4 = 0.0f;
  float sn1 = 0.0f, sn2 = 0.0f, sn3 = 0.0f, sn4 = 0.0f;
  float tn1 = 0.0f, tn2 = 0.0f, tn3 = 0.0f, tn4 = 0.0f;
  CGPoint3f calcedNormals = {0.0f, 0.0f, 0.0f};

  initVertices();

  // Hilfsmatrix befuellen (Multiplikation der Spline-Matrix mit der Geometriematrix)
  calcHelpMat();

  // Schleife ueber alle Interpolations-Punkte
  double squart = sqrt(numberOfInterpolationPoints);
  for (j = 0; j < numberOfInterpolationPoints; j += squart)
  {
    s = ((double)(j / squart) / (double)(numberOfInterpolationPoints - 1) * (squart + 1));
    for (i = 0; i < squart; i++)
    {
      t = ((double)(i) / (double)(numberOfInterpolationPoints - 1) * (squart + 1));

      coordY = calcSAndT(s, t, &s1, &s2, &s3, &s4, &t1, &t2, &t3, &t4);
      calcSnAndTn(s, t, &sn1, &sn2, &sn3, &sn4, &tn1, &tn2, &tn3, &tn4);

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

      calcNormals(&calcedNormals, qs, qt);
      Normals[i + j][NX] = calcedNormals[0];
      Normals[i + j][NY] = calcedNormals[1];
      Normals[i + j][NZ] = calcedNormals[2];

      coordX += (((-XSTARTINTPOINT) + XENDINTPOINT) / (squart - 1.0));
    }
    coordZ += (((-ZSTARTINTPOINT) + ZENDINTPOINT) / (squart - 1.0));
    coordX = XSTARTINTPOINT;
  }
  calcIndices(squart);
}

/**
 * Initialisiert die Anfangspositionen der Kugel.
 */
void initBallPosition(void)
{
  float s1 = 0.0f, s2 = 0.0f, s3 = 0.0f, s4 = 0.0f;
  float t1 = 0.0f, t2 = 0.0f, t3 = 0.0f, t4 = 0.0f;

  ballPosition[0] = Vertices[ballIndice][CX] + RADIUS + 0.001;
  ballPosition[1] = Vertices[ballIndice][CY] + RADIUS;
  ballPosition[2] = Vertices[ballIndice][CZ] + RADIUS + 0.001;

  for (int i = 0; i < 10; i++)
  {
    CGPoint3f *ball = getBall(i);
    (*ball)[1] = calcSAndT(((*ball)[2] + LOWER), ((*ball)[0] + LOWER), &s1, &s2, &s3, &s4, &t1, &t2, &t3, &t4);
  }
}

/**
 * Initialisiert die Anfangspositionen der Waende.
 */
void initWallPosition(void)
{
  float s1 = 0.0f, s2 = 0.0f, s3 = 0.0f, s4 = 0.0f;
  float t1 = 0.0f, t2 = 0.0f, t3 = 0.0f, t4 = 0.0f;

  for (int i = 0; i < 6; i++)
  {
    CGPoint3f *wall = getWall(i);
    (*wall)[1] = calcSAndT(((*wall)[2] + LOWER), ((*wall)[0] + LOWER), &s1, &s2, &s3, &s4, &t1, &t2, &t3, &t4);
  }
}

/**
 * @brief Stellt fest ob eine kollision stattfindet und 
 *        brechnet die Kollisionsnormale und die Distanz
 * 
 * @param collisionNormal die Normale der Kollisionsflaeche
 * @return die Distanz zwischen dem aeuszersten Punkt der Kugel und dem Kollisionsobjekt
 */
float detectCollision(CGPoint3f *collisionNormal)
{
  float distance = 0.0f;

  if (ballPosition[0] + RADIUS > XENDINTPOINT)
  {
    distance = ballPosition[0] + RADIUS - XENDINTPOINT;
    (*collisionNormal)[0] = 1.0f;
  }
  else if (ballPosition[0] - RADIUS < XSTARTINTPOINT)
  {
    distance = ballPosition[0] - RADIUS + XSTARTINTPOINT;
    (*collisionNormal)[0] = -1.0f;
  }
  else if (ballPosition[2] + RADIUS > ZENDINTPOINT)
  {
    distance = ballPosition[2] + RADIUS - ZENDINTPOINT;
    (*collisionNormal)[2] = 1.0f;
  }
  else if (ballPosition[2] - RADIUS < ZSTARTINTPOINT)
  {
    distance = ballPosition[2] - RADIUS + ZSTARTINTPOINT;
    (*collisionNormal)[2] = -1.0f;
  }
  float halfScaling = WALL_SCALING / 2.0;
  for (int i = 0; i < 6; i++)
  {
    CGPoint3f *wall = getWall(i);
    if (i == 2 || i == 5)
    {
      if (ballPosition[2] - RADIUS > (*wall)[2] + WALL_SCALING && 
          ballPosition[2] - RADIUS < (*wall)[2] - WALL_SCALING && 
          ballPosition[0] - RADIUS < (*wall)[0] + halfScaling && 
          ballPosition[0] + RADIUS > (*wall)[0] - halfScaling)
      {
        distance = ballPosition[2] - RADIUS - ((*wall)[2] + WALL_SCALING);
        (*collisionNormal)[2] = 1.0f;
      }
      else 
      if (ballPosition[0] < (*wall)[0] + halfScaling &&
          ballPosition[0] > (*wall)[0] - halfScaling &&
          ballPosition[2] - RADIUS < (*wall)[2] + WALL_SCALING && 
          ballPosition[2] - RADIUS > (*wall)[2] - WALL_SCALING)
      {
        distance = ballPosition[0] - RADIUS - ((*wall)[0] + halfScaling);
        (*collisionNormal)[0] = -1.0f;
      }
      else 
      if (ballPosition[0] > (*wall)[0] - halfScaling && 
          ballPosition[0] < (*wall)[0] + halfScaling && 
          ballPosition[2] + RADIUS < (*wall)[2] + WALL_SCALING && 
          ballPosition[2] + RADIUS > (*wall)[2] - WALL_SCALING)
      {
        distance = ballPosition[0] + RADIUS - (*wall)[0] - halfScaling;
        (*collisionNormal)[0] = 1.0f;
      }
      else 
      if (ballPosition[2] + RADIUS > (*wall)[2] - WALL_SCALING && 
          ballPosition[2] + RADIUS < (*wall)[2] + WALL_SCALING && 
          ballPosition[0] + RADIUS < (*wall)[0] + halfScaling && 
          ballPosition[0] - RADIUS > (*wall)[0] - halfScaling)
      {
        distance = ballPosition[2] + RADIUS - (*wall)[2] - WALL_SCALING;
        (*collisionNormal)[2] = 1.0f;
      }
      //glScalef(0.05f, 0.1f, 0.1f);
    }
    else
    {
      if (ballPosition[2] + RADIUS > (*wall)[2] - halfScaling && 
          ballPosition[2] + RADIUS < (*wall)[2] + halfScaling && 
          ballPosition[0] - RADIUS < (*wall)[0] + WALL_SCALING && 
          ballPosition[0] + RADIUS > (*wall)[0] - WALL_SCALING)
      {
        distance = ballPosition[2] + RADIUS - (*wall)[2] - halfScaling;
        (*collisionNormal)[2] = -1.0f;
      }
      else 
      if (ballPosition[0] - RADIUS < (*wall)[0] + WALL_SCALING &&
          ballPosition[0] - RADIUS > (*wall)[0] - WALL_SCALING &&
          ballPosition[2] < (*wall)[2] + halfScaling && 
          ballPosition[2] > (*wall)[2] - halfScaling)
      {
        distance = ballPosition[0] - RADIUS - ((*wall)[0] + WALL_SCALING);
        (*collisionNormal)[0] = 1.0f;
      }
      else 
      if (ballPosition[0] + RADIUS > (*wall)[0] - WALL_SCALING && 
          ballPosition[0] + RADIUS < (*wall)[0] + WALL_SCALING && 
          ballPosition[2] < (*wall)[2] + halfScaling && 
          ballPosition[2] > (*wall)[2] - halfScaling)
      {
        distance = ballPosition[0] + RADIUS - (*wall)[0] - WALL_SCALING;
        (*collisionNormal)[0] = -1.0f;
      }
      else 
      if (ballPosition[2] - RADIUS < (*wall)[2] + halfScaling && 
          ballPosition[2] - RADIUS > (*wall)[2] - halfScaling && 
          ballPosition[0] - RADIUS < (*wall)[0] + WALL_SCALING && 
          ballPosition[0] + RADIUS > (*wall)[0] - WALL_SCALING)
      {
        distance = (ballPosition[2] - RADIUS) + ((*wall)[2] + halfScaling);
        (*collisionNormal)[2] = 1.0f;
      }
      //glScalef(0.1f, 0.1f, 0.05f);
    }
  }

  return distance;
}

/**
 * Berechnet die Bewegung einer rollenden Kugel mittels Euler-Integration.
 * @param interval das aktuelle Zeitintervall
 */
void calcEulerIntegration(double interval)
{
  float s1 = 0.0f, s2 = 0.0f, s3 = 0.0f, s4 = 0.0f;
  float t1 = 0.0f, t2 = 0.0f, t3 = 0.0f, t4 = 0.0f;
  float sn1 = 0.0f, sn2 = 0.0f, sn3 = 0.0f, sn4 = 0.0f;
  float tn1 = 0.0f, tn2 = 0.0f, tn3 = 0.0f, tn4 = 0.0f;

  CGVector3f calcedNormals = {0.0f, 0.0f, 0.0f};
  CGPoint2f l = {0.0f, 0.0f};
  CGPoint2f force = {0.0f, 0.0f};
  CGPoint2f acceleration = {0.0f, 0.0f};
  CGPoint3f collisionNormal = {0.0f, 0.0f, 0.0f};

  ballPosition[1] = calcSAndT((ballPosition[2] + LOWER), (ballPosition[0] + LOWER), &s1, &s2, &s3, &s4, &t1, &t2, &t3, &t4);

  float distance = detectCollision(&collisionNormal);

  calcSnAndTn((ballPosition[2] + LOWER), (ballPosition[0] + LOWER), &sn1, &sn2, &sn3, &sn4, &tn1, &tn2, &tn3, &tn4);

  float qs = (1.0 / 36.0) * ((sn1 * t1) + (sn2 * t2) + (sn3 * t3) + (sn4 * t4));
  float qt = (1.0 / 36.0) * ((s1 * tn1) + (s2 * tn2) + (s3 * tn3) + (s4 * tn4));

  calcNormals(&calcedNormals, qs, qt);

  ballPosition[3] = calcedNormals[NX];
  ballPosition[4] = calcedNormals[NY];
  ballPosition[5] = calcedNormals[NZ];

  l[0] = calcedNormals[NX] * (GRAVITY * calcedNormals[NY]);
  l[1] = calcedNormals[NZ] * (GRAVITY * calcedNormals[NY]);

  force[0] = -l[0];
  force[1] = -l[1];

  acceleration[0] = force[0] / MASS;
  acceleration[1] = force[1] / MASS;

  if (distance)
  {
    acceleration[0] += ((distance * SPRINGCONSTANT) * (collisionNormal)[NX]) / MASS;
    acceleration[1] += ((distance * SPRINGCONSTANT) * (collisionNormal)[NZ]) / MASS;
    acceleration[0] = calcedNormals[0] < 0.0 ? acceleration[0] : -acceleration[0];
    acceleration[1] = calcedNormals[2] < 0.0 ? acceleration[1] : -acceleration[1];
  }

  ballPosition[6] = (ballPosition[6] + interval * acceleration[0]) * FRICTION;
  ballPosition[7] = (ballPosition[7] + interval * acceleration[1]) * FRICTION;

  ballPosition[0] = (ballPosition[0]) + interval * ballPosition[6];
  ballPosition[2] = (ballPosition[2]) + interval * ballPosition[7];

  ballPosition[1] = calcSAndT((ballPosition[2] + LOWER), (ballPosition[0] + LOWER), &s1, &s2, &s3, &s4, &t1, &t2, &t3, &t4);
}

/**
 * Berechnung der Position der Lichtquelle.
 * @param interval das aktuelle Zeitintervall
 */
void calcLightPos(double interval)
{
  /* Lichtquelle */
  lightPos[0] = LIGHT_DISTANCE * sinf(interval);
  lightPos[1] = 0.5f;
  lightPos[2] = LIGHT_DISTANCE * cosf(interval);
  lightPos[3] = 1.0;
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
  initWallPosition();
}

/**
 * Wechselt zwischen den steuerbaren Waenden
 * @param idx Index der Wand
 */
void switchWall(int idx)
{
  selectWall = idx; 
}

/**
 * Veraendert die Position der ausgewaehlten Wand
 * @param direction 0 Rechts / 1 Unten / 2 Links / 3 Oben
 */
void moveWall(int direction)
{
  CGPoint3f *wall = getWall(selectWall);
  switch (direction)
  {
  case 0:
    (*wall)[0] += 0.05f;
    break;
  case 1:
    (*wall)[2] += 0.05f;
    break;
  case 2:
    (*wall)[0] -= 0.05f;
    break;
  case 3:
    (*wall)[2] -= 0.05f;
    break;
  }
  initWallPosition();
}


/**
 * @brief liefert die Wandkoordinaten
 * 
 * @param count index der Wand
 * @return CGPoint3f* die Wandkoordinaten
 */
CGPoint3f *getWall(int count)
{
  CGPoint3f *result;
  switch (count)
  {
  case 0:
    result = &wall0;
    break;
  case 1:
    result = &wall1;
    break;
  case 2:
    result = &wall2;
    break;
  case 3:
    result = &wall3;
    break;
  case 4:
    result = &wall4;
    break;
  case 5:
    result = &wall5;
    break;
  }
  return result;
}

/**
 * Aendert den Toggle fuer die Kontrollpunkteanzeige.
 */
void pauseToggle(void)
{
  togglePause = (togglePause + 1) % 2;
}

/**
 * Aendert den Toggle fuer Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void toggleHelp(void)
{
  helpToggle = (helpToggle + 1) % 2;
}

/**
 * Aendert den Toggle fuer Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void switchStart(void)
{
  start = 1;
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
int *getSelectWall(void)
{
  return &selectWall;
}

/*
 * Gibt die Lichtposition zurück.
 */
CGPoint4f *getLightPos(void)
{
  return &lightPos;
}

/*
 * Gibt die Position der Kugel zurück.
 */
CGPoint8f *getBallPos(void)
{
  return &ballPosition;
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
 * liefert den Wert von Start zurueck.
 */
int *getStart(void)
{
  return &start;
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
  return &texture;
}

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

/**
 * @return Liefert den aktuellen Kontrollpunkt, der veraendert werden kann.
 */
int *getPause(void)
{
  return &togglePause;
}