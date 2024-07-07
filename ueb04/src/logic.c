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

static int helpToggle = 0;
static int togglePause = 1;

static int texture = 0;
static int particleType = 0;
static int camera = 1;

CGPoint4f lightPos = {0.0f, 0.5f, 0.0f, 1.0f};

static int numberOfParticles = 160;
static int activeParticles = 0;

static int countOfGoals = 1;

CGPoint3f redBallPosition = {-0.5f, 0.0f, 0.0f};
CGPoint3f blueBallPosition = {0.5f, 0.0f, 0.0f};

// Array für die Partikel
Vertex *Particles;

/**
 * Berechnet die Lenge aus eines Vektors
 * @param des die Ergebnislenge
 * @param a der Vektor
 */
void length(float *des, CGVector3f a)
{
  (*des) = sqrt(pow(a[0], 2.0) + pow(a[1], 2.0) + pow(a[2], 2.0));
}

/**
 * Berechnet das Skalarprodukt aus zwei Vektoren
 * @param des die Skalarprodukt
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void dot(float *des, CGVector3f a, CGVector3f b)
{
  (*des) = (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

/**
 * Berechnet das Kreuzprodukt aus zwei Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void cross(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[1] * b[2] - a[2] * b[1];
  (*des)[1] = a[2] * b[0] - a[0] * b[2];
  (*des)[2] = a[0] * b[1] - a[1] * b[0];
}

/**
 * Setzt einen Vektor komponentenweise ein
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 */
void setVecf(CGVector3f *des, float a)
{
  (*des)[0] = a;
  (*des)[1] = a;
  (*des)[2] = a;
}

/**
 * Setzt einen Vektor komponentenweise ein
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 */
void setVecV(CGVector3f *des, CGVector3f a)
{
  (*des)[0] = a[0];
  (*des)[1] = a[1];
  (*des)[2] = a[2];
}

/**
 * Berechnet die Addition zweier Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecAddVec(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[0] + b[0];
  (*des)[1] = a[1] + b[1];
  (*des)[2] = a[2] + b[2];
}

/**
 * Berechnet die Subtraktion zweier Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecSubVec(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[0] - b[0];
  (*des)[1] = a[1] - b[1];
  (*des)[2] = a[2] - b[2];
}

/**
 * Berechnet die Multiplikation zweier Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecMulVec(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[0] * b[0];
  (*des)[1] = a[1] * b[1];
  (*des)[2] = a[2] * b[2];
}

/**
 * Berechnet die Division zweier Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecDivVec(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[0] / b[0];
  (*des)[1] = a[1] / b[1];
  (*des)[2] = a[2] / b[2];
}

/**
 * Berechnet die Addition eines Vektors und eines Skalars
 * @param des der Ergebnisvektor
 * @param a Vektor
 * @param b Skalar
 */
void vecAddSca(CGVector3f *des, CGVector3f a, float b)
{
  (*des)[0] = a[0] + b;
  (*des)[1] = a[1] + b;
  (*des)[2] = a[2] + b;
}

/**
 * Berechnet die Subtraktion eines Vektors und eines Skalars
 * @param des der Ergebnisvektor
 * @param a Vektor
 * @param b Skalar
 */
void vecSubSca(CGVector3f *des, CGVector3f a, float b)
{
  (*des)[0] = a[0] - b;
  (*des)[1] = a[1] - b;
  (*des)[2] = a[2] - b;
}

/**
 * Berechnet die Multiplikation eines Vektors und eines Skalars
 * @param des der Ergebnisvektor
 * @param a Vektor
 * @param b Skalar
 */
void vecMulSca(CGVector3f *des, CGVector3f a, float b)
{
  (*des)[0] = a[0] * b;
  (*des)[1] = a[1] * b;
  (*des)[2] = a[2] * b;
}

/**
 * Berechnet die Division eines Vektors und eines Skalars
 * @param des der Ergebnisvektor
 * @param a Vektor
 * @param b Skalar
 */
void vecDivSca(CGVector3f *des, CGVector3f a, float b)
{
  (*des)[0] = a[0] / b;
  (*des)[1] = a[1] / b;
  (*des)[2] = a[2] / b;
}

/**
 * Berechnet eine Zufallszahl zwischen Maximum und Minimum
 * @param des das Ergebnis
 * @param max Maximum
 * @param min Minimum
 */
void randomMaxMin(float *des, int max, int min)
{
  (*des) = (float)(rand() % (max + 1 - min)) + min;
  (*des) /= 10.0f;
}

/**
 * Hilfsfunktion zur Berechnung der Beschleunigung.
 * @param tMuinusS Differenz zwischen Zielposition und Position
 * @param goalPosition die Zielposition
 * @param position die Position
 * @param lenghtOfVec Laenge des Vektors
 * @param acceleration die Beschleunigung
 */
void calcAcceleration(CGVector3f *tMinusS, CGPoint3f *goalPosition, CGVector3f *position, float *lengthOfVec, CGPoint3f *acceleration)
{
  vecSubVec(tMinusS, (*goalPosition), (*position));
  length(lengthOfVec, (*tMinusS));
  vecDivSca(acceleration, (*tMinusS), (*lengthOfVec));
  vecMulSca(acceleration, (*acceleration), KWEAK);
}

/**
 * Reserviert den Speicher für das Partikel-Array.
 */
void initMemory(void)
{
  int *numberOfParticles = getNumberOfParticles();
  Particles = malloc(sizeof(Vertex) * (*numberOfParticles));
}

/**
 * Leert den Speicher fuer das Partikel-Array.
 */
void freeAll(void)
{
  free(Particles);
  Particles = NULL;
}

/**
 * Initialisiert die Anfangswerte der Partikel.
 */
void initParticles(void)
{
  int i = 0;
  float x = 0;
  float y = 0;
  float z = 0;
  for (i = 0; i < numberOfParticles; i++)
  {
    randomMaxMin(&x, 10, -10);
    randomMaxMin(&y, 10, -10);
    randomMaxMin(&z, 10, -10);
    Particles[i][CX] = x;
    Particles[i][CY] = y;
    Particles[i][CZ] = z;
    Particles[i][OX] = 0.0f;
    Particles[i][OY] = 0.0f;
    Particles[i][OZ] = 0.0f;
    Particles[i][VX] = 0.0f;
    Particles[i][VY] = 0.0f;
    Particles[i][VZ] = 0.0f;
  }
}

/**
 * Berechnet die Bewegung der Partikel mittels Euler-Integration.
 * @param interval Das Zeitinterval zwischen den Aufrufen der Funktion
 */
void calcEulerIntegration(double interval)
{
  int i = 0;
  float lengthOfVec = 0.0f;
  float g = 0.0f;
  float kV = 0.0f;
  CGVector3f position = {0.0f, 0.0f, 0.0f};
  CGVector3f velocity = {0.0f, 0.0f, 0.0f};
  CGPoint3f acceleration = {0.0f, 0.0f, 0.0f};
  CGPoint3f accelAll = {0.0f, 0.0f, 0.0f};
  CGPoint3f vEuler = {0.0f, 0.0f, 0.0f};
  CGVector3f tMinusS = {0.0f, 0.0f, 0.0f};
  CGVector3f upVec = {0.0f, 0.0f, 0.0f};
  CGPoint3f goalPosition = {0.0f, 0.0f, 0.0f};
  CGPoint3f particelCenter = {0.0f, 0.0f, 0.0f};

  for (i = 0; i < numberOfParticles; i++)
  {

    setVecf(&accelAll, 0.0f);

    position[CX] = Particles[i][CX];
    position[CY] = Particles[i][CY];
    position[CZ] = Particles[i][CZ];

    velocity[CX] = Particles[i][VX];
    velocity[CY] = Particles[i][VY];
    velocity[CZ] = Particles[i][VZ];

    if (countOfGoals == 0)
    {
      for (int j = 0; j < numberOfParticles; j++)
      {
        particelCenter[CX] += Particles[j][CX];
        particelCenter[CY] += Particles[j][CY];
        particelCenter[CZ] += Particles[j][CZ];
      }
      vecDivSca(&particelCenter, particelCenter, numberOfParticles);
      setVecV(&goalPosition, particelCenter);

      // Beschleunigungsberechnung
      calcAcceleration(&tMinusS, &goalPosition, &position, &lengthOfVec, &acceleration);
    }
    if (countOfGoals == 1)
    {
      goalPosition[CX] = Particles[activeParticles][CX];
      goalPosition[CY] = Particles[activeParticles][CY];
      goalPosition[CZ] = Particles[activeParticles][CZ];
      if (i != activeParticles)
      {
        // Beschleunigungsberechnung
        calcAcceleration(&tMinusS, &goalPosition, &position, &lengthOfVec, &acceleration);
      }
    }
    if (countOfGoals >= 2 || (countOfGoals == 1 && i == activeParticles))
    {
      for (int j = 0; j < 2; j++)
      {
        if (j == 0)
        {
          setVecV(&goalPosition, redBallPosition);
        }
        else if (j == 1)
        {
          setVecV(&goalPosition, blueBallPosition);
        }
        // Gewichteberechnung
        vecSubVec(&tMinusS, goalPosition, position);
        length(&lengthOfVec, tMinusS);
        lengthOfVec = pow(lengthOfVec, 2);
        lengthOfVec /= GAUSZ;
        g = pow(EULER, (-lengthOfVec));

        // Einzelbeschleunigung
        calcAcceleration(&tMinusS, &goalPosition, &position, &lengthOfVec, &acceleration);

        // Summe
        vecMulSca(&acceleration, acceleration, g);
        vecAddVec(&accelAll, accelAll, acceleration);
      }
      setVecV(&acceleration, accelAll);
    }

    // Geschwindigkeitsberechnung
    vecMulSca(&vEuler, acceleration, interval);
    vecAddVec(&vEuler, vEuler, velocity);

    length(&lengthOfVec, vEuler);
    vecDivSca(&vEuler, vEuler, lengthOfVec);

    if (i == activeParticles)
    {
      kV = 2.0;
    }
    else
    {
      kV = KV;
    }

    vecMulSca(&velocity, vEuler, kV);

    if (togglePause)
    {
      Particles[i][VX] = velocity[CX];
      Particles[i][VY] = velocity[CY];
      Particles[i][VZ] = velocity[CZ];
    }

    //Orts Berechnung
    Particles[i][CX] = Particles[i][CX] + interval * Particles[i][VX];
    Particles[i][CY] = Particles[i][CY] + interval * Particles[i][VY];
    Particles[i][CZ] = Particles[i][CZ] + interval * Particles[i][VZ];

    //UP Vektor Berechnung
    cross((&upVec), velocity, acceleration);
    cross((&upVec), upVec, velocity);

    if (togglePause)
    {
      Particles[i][OX] = upVec[CX];
      Particles[i][OY] = upVec[CY];
      Particles[i][OZ] = upVec[CZ];
    }
  }
}

/**
 * Aendert den Toggle fuer die Pause.
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
 * Erhoeht die Anzahl der Partikel.
 */
void addParticle(void)
{
  numberOfParticles++;
}

/**
 * Verringert die Anzahl der Partikel.
 */
void deleteParticle(void)
{
  if (numberOfParticles > 0)
  {
    numberOfParticles--;
  }
}

/*
 * Gibt den Status fuer die Hilfeausgabe zurueck.
 * @return den Status
 */
int *getHelp(void)
{
  return &helpToggle;
}

/*
 * Gibt die Position der roten Kugel zurück.
 * @return die Position der roten Kugel
 */
CGPoint3f *getRedBallPos(void)
{
  return &redBallPosition;
}

/*
 * Gibt die Position der blauen Kugel zurück.
 * @return die Position der blauen Kugel
*/
CGPoint3f *getBlueBallPos(void)
{
  return &blueBallPosition;
}

/*
 * Aendert den Switch fuer die Textur.
 */
void switchTexture(void)
{
  texture = (texture + 1) % 2;
}

/*
 * Gibt die aktuelle Textur zurueck.
 * @return die Textur
 */
int *getTexture(void)
{
  return &texture;
}

/**
 * Gibt die Anzahl aller Partikel zurueck.
 * @return die Anzahl der Partikel
 */
int *getNumberOfParticles(void)
{
  return &numberOfParticles;
}

/**
 * Liefert das Partikel-Array zurueck.
 * @return Das Partikel-Array.
 */
Vertex *
getParticles(void)
{
  return Particles;
}

/*
 * Liefert das ausgewaehlte Partikel zurueck.
 * @return das aktive Partikel
 */
int *getActiveParticle(void)
{
  return &activeParticles;
}

/**
 * Liefert den Partikel-Typ zurueck.
 * @return Den Partikel-Typ.
 */
int *getParticleType(void)
{
  return &particleType;
}

/**
 * Aendert den Partikel-Typ.
 */
void switchParticleType(void)
{
  particleType = (particleType + 1) % 2;
}

/**
 * Liefert den Status der Pause.
 * @return der Status
 */
int *getPause(void)
{
  return &togglePause;
}

/*
 * Aendert das ausgewaehlte Partikel.
 */
void switchParticel(void)
{
  activeParticles = (activeParticles + 1) % numberOfParticles;
}

/**
 * Gibt die Lichtposition zurueck.
 * @return die Lichtposition
 */
CGPoint4f *getLightPos(void)
{
  return &lightPos;
}

/**
 * Gibt die Kameraposition zurueck.
 * @return die Kameraposition
 */
int *getCamera(void)
{
  return &camera;
}

/**
 * Aendert die Kamera.
 */
void switchCamera(void)
{
  camera = (camera + 1) % 2;
}

/**
 * Wechselt das Ziel der Partikel.
 */
void switchTarget(void)
{
  countOfGoals = (countOfGoals + 1) % 3;
}