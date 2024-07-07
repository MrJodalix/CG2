/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * @author Joshua-Scott Schoettke, Ilana Schmara (Gruppe 1).
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glut.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "logic.h"
#include "types.h"
#include "debugGL.h"

/* ---- Globale Daten ---- */

/**
 * Zeichen-Funktion zur Erstellung einer Einheitslinie.
 */
void drawUnitLine(float *velo)
{
  glBegin(GL_LINES);
  {
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(-velo[0], -velo[1], -velo[2]);
  }
  glEnd();
}

/**
 * Zeichen-Funktion zur Erstellung des Einheitsquadrats.
 */
void drawUnitSquare(void)
{
  glPushMatrix();
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    {
      glVertex2d(0.5, 0.5);
      glVertex2d(-0.5, 0.5);
      glVertex2d(-0.5, -0.5);
      glVertex2d(0.5, -0.5);
    }
    glEnd();
  }
  glPopMatrix();
}

/**
 * Zeichen-Funktion fuer ein gleichseitiges Einheitsdreieck.
 */
void drawUnitEquilateralTriangle(float pos1, float pos2, float pos3)
{
  glBegin(GL_TRIANGLES);
  {
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(pos1, 0.0, 0.0);
    glVertex3d(0.0, pos2, 0.0);
  }
  glEnd();
}

/**
 * Zeichen-Funktion zur Erstellung der Einheitskugel.
 */
void drawUnitSphere(void)
{
  GLUquadricObj *unitCircle = 0;
  unitCircle = gluNewQuadric();
  gluQuadricNormals(unitCircle, 1);
  gluSphere(unitCircle, 1, 32, 32);
  gluDeleteQuadric(unitCircle);
}

/**
 * Zeichenfunktion zu Erstellung der blauen Kugel.
 * @param blueBallPos die Position der blauen Kugel
 */
void drawBlueBallSphere(float *blueBallPos)
{
  // Material: blau
  float matAmbient[] = {0.0, 0.0, 1.0, 1.0};
  float matDiffuse[] = {0.0, 0.0, 0.5, 1.0};
  float matSpecular[] = {0.0, 0.0, 0.1, 1.0};
  float matShininess = 1;

  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);

  glPushMatrix();
  {
    glColor3f(0.0f, 0.0f, 1.0f);
    glTranslatef(blueBallPos[0], blueBallPos[1], blueBallPos[2]);
    glScalef(RADIUS * 2, RADIUS * 2, RADIUS * 2);
    drawUnitSphere();
  }
  glPopMatrix();
}

/**
 * Zeichnet die Wände am Rand der Spline-Fläche.
 */
void drawBorder(void)
{
  float matAmbient[] = {0.5, 0.5, 0.5, 1.0};
  float matDiffuse[] = {0.3, 0.3, 0.3, 1.0};
  float matSpecular[] = {0.2, 0.2, 0.2, 1.0};
  float matShininess = 0.5;

  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);
  glColor3f(1.0, 1.0, 1.0);

  glPushMatrix();
  {
    glTranslatef(0.0f, 0.0f, GROUND);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(LEFT, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.0f, 0.0f, CEILING);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(RIGHT, 0.0f, 0.0f);
    glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.0f, CEILING, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.0f, GROUND, 0.0f);
    glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
}

/**
 * Zeichenfunktion zu Erstellung der blauen Kugel.
 * @param blueBallPos die Position der blauen Kugel
 */
void drawRedCube(float *redCubePos)
{
  // Material: blau
  float matAmbient[] = {1.0, 0.0, 0.0, 1.0};
  float matDiffuse[] = {0.5, 0.0, 0.0, 1.0};
  float matSpecular[] = {0.1, 0.0, 0.0, 1.0};
  float matShininess = 1;

  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);

  glPushMatrix();
  {
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(redCubePos[0], redCubePos[1], redCubePos[2]);
    glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
    glScalef(0.3f, 0.3f, 0.3f);
    drawBorder();
  }
  glPopMatrix();
}

/**
 * Zeichen-Funktion fuer jeden Frame.
 */
void drawScene(void)
{
  CGPoint3f *blueBallPos = getBlueBallPos();
  CGPoint3f *redCubePos = getRedCubePos();

  /* Lichtberechnung aktivieren */
  glEnable(GL_LIGHTING);

  CGPoint4f *lightPos0 = getLightPos(0);
  CGPoint4f *lightPos1 = getLightPos(1);

  if (getLightActive(0))
  {
    /* Position der Lichtquelle setzen */
    glLightfv(GL_LIGHT0, GL_POSITION, (*lightPos0));
    /* Erste Lichtquelle aktivieren */
    glEnable(GL_LIGHT0);
  }
  else
  {
    glDisable(GL_LIGHT0);
  }

  if (getLightActive(1))
  {
    /* Position der Lichtquelle setzen */
    glLightfv(GL_LIGHT1, GL_POSITION, (*lightPos1));
    /* Erste Lichtquelle aktivieren */
    glEnable(GL_LIGHT1);
  }
  else
  {
    /* Erste Lichtquelle aktivieren */
    glDisable(GL_LIGHT1);
  }

  // TODO Texture Koordinaten festlegen
  glPushMatrix();
  {
    glEnable(GL_COLOR_MATERIAL);
    drawBorder();
    glDisable(GL_COLOR_MATERIAL);
  }
  glPopMatrix();

  if (getDrawBlue())
  {
    glPushMatrix();
    {
      glEnable(GL_COLOR_MATERIAL);
      drawBlueBallSphere((*blueBallPos));
      glDisable(GL_COLOR_MATERIAL);
    }
    glPopMatrix();
  }
  if (getDrawRed())
  {
    glPushMatrix();
    {
      glEnable(GL_COLOR_MATERIAL);
      drawRedCube((*redCubePos));
      glDisable(GL_COLOR_MATERIAL);
    }
    glPopMatrix();
  }
  /* Lichtberechnung deaktivieren */
  glDisable(GL_LIGHTING);

  /* DEBUG-Ausgabe */
  // GLSTATE;
}

/**
 * Initialisierung der Lichtquellen.
 * Setzt Eigenschaften der Lichtquellen (Farbe, Oeffnungswinkel, ...)
 */
void initLight(void)
{
  /* Farbe der Lichtquelle */
  CGPoint4f lightColor0[3] =
      {{0.3f, 0.3f, 0.3f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.8f, 0.8f, 0.8f, 1.0f}};

  /* Farbe der Lichtquelle setzen */
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor0[0]);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0[1]);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0[2]);

  /* Farbe der Lichtquelle setzen */
  glLightfv(GL_LIGHT1, GL_AMBIENT, lightColor0[0]);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor0[1]);
  glLightfv(GL_LIGHT1, GL_SPECULAR, lightColor0[2]);
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrundfarbe.
 * Initialisiert die Displaylisten.
 * Initialisiert das Vertex-Array und berechnet die Kurve.
 * Setzt die Startposition der Rakete.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void)
{
  /* Setzen der Farbattribute */
  /* Hintergrundfarbe */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  /* Z-Buffer-Test aktivieren */
  glEnable(GL_DEPTH_TEST);

  /* Polygonrueckseiten nicht anzeigen */
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  /* Normalen nach Transformationen wieder auf die
   * Einheitslaenge bringen */
  glEnable(GL_NORMALIZE);

  initLight();

  initQuadStruct();
  
  raycasting();

  /* Alles in Ordnung? */
  return (GLGETERROR == GL_NO_ERROR);
}