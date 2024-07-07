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
#include "stringOutput.h"
#include "texture.h"

/**
 * Zeichen-Funktion zur Erstellung einer Parikellinie.
 */
void drawParticleLine(float *velo, int active)
{
  glBegin(GL_LINES);
  {
    if ((*getActiveParticle()) == active)
    {
      glColor3f(1.0, 0.0, 0.0);
    }
    else
    {
      glColor3f(0.0, 1.0, 0.0);
    }
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(-velo[0] / 2.0, -velo[1] / 2.0, -velo[2] / 2.0);

    if ((*getActiveParticle()) == active)
    {
      glColor3f(0.0, 0.0, 1.0);
    }
    else
    {
      glColor3f(0.0, 1.0, 1.0);   
    }
    glVertex3d(-velo[0] / 2.0, -velo[1] / 2.0, -velo[2] / 2.0);
    glVertex3d(-velo[0], -velo[1], -velo[2]);
  }
  glEnd();
}

/**
 * Zeichen-Funktion zur Erstellung des Einheitsquadrats mit Texturkoordinaten.
 */
void drawUnitSquare(void)
{
  glPushMatrix();
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    {
      glTexCoord2f(0.0f, 0.0f);
      glVertex2d(0.5, 0.5);
      glTexCoord2f(0.0f, 1.0f);
      glVertex2d(-0.5, 0.5);
      glTexCoord2f(1.0f, 1.0f);
      glVertex2d(-0.5, -0.5);
      glTexCoord2f(1.0f, 0.0f);
      glVertex2d(0.5, -0.5);
    }
    glEnd();
  }
  glPopMatrix();
}

/**
 * Zeichen-Funktion fuer ein Partikel-Dreieck.
 */
void drawParticleTriangle(float *velo, int active)
{
  glBegin(GL_TRIANGLES);
  {
    if ((*getActiveParticle()) == active)
    {
      glColor3f(1.0, 0.0, 0.0);
    }
    else
    {
      glColor3f(0.0, 1.0, 0.0);
    }
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(-velo[0], -velo[1], -velo[2]);
    glVertex3d(-velo[0] * 2.0, -velo[1], -velo[2]);

    if ((*getActiveParticle()) == active)
    {
      glColor3f(0.0, 0.0, 1.0);
    }
    else
    {
      glColor3f(0.0, 1.0, 1.0);
    }
    glVertex3d(-velo[0], -velo[1], -velo[2]);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(-velo[0] * 0.5, -velo[1], -velo[2]);
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
 * Zeichenfunktion zu Erstellung der roten Kugel.
 * @param redBallPos die Position der roten Kugel
 */
void drawRedBallSphere(float *redBallPos)
{
  // Material: rot
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
    glTranslatef(redBallPos[0], redBallPos[1], redBallPos[2]);
    glScalef(0.1f, 0.1f, 0.1f);
    drawUnitSphere();
  }
  glPopMatrix();
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
    glColor3f(0.0, 0.0, 1.0);
    glTranslatef(blueBallPos[0], blueBallPos[1], blueBallPos[2]);
    glScalef(0.075f, 0.075f, 0.075f);
    drawUnitSphere();
  }
  glPopMatrix();
}

/**
 * Zeichenfunktion fuer ein Partikel.
 * @param position die Position des Partikels
 * @param velocity die Ausrichtung des Partikels
 * @param active das aktive Partikel
 */
void drawParticle(float *position, float *velocity, int active)
{
  // Material: gruen
  float matAmbient[] = {0.0, 1.0, 0.0, 1.0};
  float matDiffuse[] = {0.0, 0.5, 0.0, 1.0};
  float matSpecular[] = {0.0, 0.1, 0.0, 1.0};
  float matShininess = 1;

  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);

  glPushMatrix();
  {
    glColor3f(0.0, 1.0, 0.0);
    glTranslatef(position[0], position[1], position[2]);
    if ((*getParticleType()))
    {
      glScalef(0.075f, 0.075f, 0.075f);
      drawParticleTriangle(velocity, active);
    }
    else
    {
      glScalef(0.075f, 0.075f, 0.075f);
      drawParticleLine(velocity, active);
    }
  }
  glPopMatrix();
}

/**
 * Zeichen-Hilfsfunktion fuer alle Partikel.
 */
void drawParticles()
{
  Vertex *particles = getParticles();
  int *numberOfParticles = getNumberOfParticles();
  CGPoint3f pos = {0.0f, 0.0f, 0.0f};
  CGPoint3f velo = {0.0f, 0.0f, 0.0f};
  for (int i = 0; i < (*numberOfParticles); i++)
  {
    pos[0] = particles[i][CX];
    pos[1] = particles[i][CY];
    pos[2] = particles[i][CZ];
    velo[0] = particles[i][VX];
    velo[1] = particles[i][VY];
    velo[2] = particles[i][VZ];

    drawParticle(pos, velo, i); 
  }
}

/**
 * Zeichnet die Waende.
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
    glTranslatef(0.0f, 0.0f, ZSTARTINTPOINT);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(XSTARTINTPOINT, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.0f, 0.0f, ZENDINTPOINT);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(XENDINTPOINT, 0.0f, 0.0f);
    glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.0f, 1.0, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
}

/**
 * Zeichen-Funktion fuer den Boden.
 */
void drawLand(void)
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
    glTranslatef(0.0f, -1.0, 0.0f);
    glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    drawUnitSquare();
  }
  glPopMatrix();
}

/**
 * Zeichen-Funktion fuer den Hilfe-Screen.
 */
void drawHelp(void)
{
  GLfloat textColor[3] = {1.0f, 0.0f, 0.0f};

  drawString(0.05f, 0.1f, textColor, "Tastenbelegung");
  drawString(0.05f, 0.2f, textColor, "q/Q/Esc : Beendet das Spiel.");
  drawString(0.05f, 0.25f, textColor, "h/H : Ruft diese Hilfeausgabe auf und pausiert das Spiel.");
  drawString(0.05f, 0.3f, textColor, "p/P : Pause");
  drawString(0.05f, 0.35f, textColor, "d/D : Wechselt zwischen Darstellungsformen");
  drawString(0.05f, 0.4f, textColor, "t/T : Textur wechseln");
  drawString(0.05f, 0.45f, textColor, "z/Z : Schaltet zwischen den Zielen durch");
  drawString(0.05f, 0.5f, textColor, "n/N : Schalten zwischen den Partikeln");
  drawString(0.05f, 0.55f, textColor, "Pfeiltasten links : Partikel verringern");
  drawString(0.05f, 0.6f, textColor, "Pfeiltasten rechts : Partikel erhoehen");
  drawString(0.05f, 0.65f, textColor, "F1 : Schaltet zwischen normalem und WireFrame/-Mode");
  drawString(0.05f, 0.7f, textColor, "F2 : Waehle zwischen Full-Frame und Window-Frame");
}

/**
 * Zeichen-Funktion fuer jeden Frame.
 */
void drawScene(void)
{
  int *help = getHelp();
  CGPoint3f *redBallPos = getRedBallPos();
  CGPoint3f *blueBallPos = getBlueBallPos();

  // Licht
  glEnable(GL_LIGHTING);
  CGPoint4f *lightPos = getLightPos();
  glLightfv(GL_LIGHT0, GL_POSITION, (*lightPos));
  glEnable(GL_LIGHT0);

  // Auszenwaende zeichnen
  glEnable(GL_TEXTURE_2D);
  switch ((*getTexture()))
  {
  case 0:
    bindTexture(texDay);
    break;
  case 1:
    bindTexture(texNight);
    break;
  }
  glEnable(GL_COLOR_MATERIAL);
  drawBorder();
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_TEXTURE_2D);

  // Boden zeichnen
  glEnable(GL_TEXTURE_2D);
  bindTexture(texLandscape);
  glEnable(GL_COLOR_MATERIAL);
  drawLand();
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_TEXTURE_2D);

  // Zielkugeln zeichnen
  glPushMatrix();
  {
    glEnable(GL_COLOR_MATERIAL);
    drawRedBallSphere((*redBallPos));
    glDisable(GL_COLOR_MATERIAL);
  }
  glPopMatrix();

  glPushMatrix();
  {
    glEnable(GL_COLOR_MATERIAL);
    drawBlueBallSphere((*blueBallPos));
    glDisable(GL_COLOR_MATERIAL);
  }
  glPopMatrix();

  // Partikel zeichnen
  glDisable(GL_CULL_FACE);
  glEnable(GL_COLOR_MATERIAL);
  drawParticles();
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);

  if ((*help))
  {
    drawHelp();
  }

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
  initMemory();
  initParticles();

  /* Alles in Ordnung? */
  return (GLGETERROR == GL_NO_ERROR);
}

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void)
{
  /* Flag: Wireframe: ja/nein */
  static GLboolean wireframe = GL_FALSE;

  /* Modus wechseln */
  wireframe = !wireframe;

  if (wireframe)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}