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

/* ---- Globale Daten ---- */

// Meteoriten-Displaylist-ID
static GLuint g_meteorListID = 1;

/**
 * Zeichen-Funktion zur Erstellung des Einheitsquadrats.
 */
void drawUnitSquare(void)
{
  glBegin(GL_QUADS);
  {
    glVertex2d(0.5, 0.5);
    glVertex2d(-0.5, 0.5);
    glVertex2d(-0.5, -0.5);
    glVertex2d(0.5, -0.5);
  }
  glEnd();
}

/**
 * Zeichen-Funktion zur Erstellung des Einheitskreises.
 */
void drawUnitCircle(void)
{
  GLUquadricObj *unitCircle = 0;
  unitCircle = gluNewQuadric();
  gluDisk(unitCircle, 0, 1, 32, 1);
  gluDeleteQuadric(unitCircle);
}

/**
 * Zeichen-Funktion zur Erstellung eines groben Einheitskreises.
 */
void drawRoughUnitCircle(void)
{
  GLUquadricObj *unitCircle = 0;
  unitCircle = gluNewQuadric();
  gluDisk(unitCircle, 0, 1, 12, 1);
  gluDeleteQuadric(unitCircle);
}

/**
 * Zeichen-Funktion fuer ein gleichseitiges Einheitsdreieck.
 */
void drawUnitTriangle(void)
{
  GLUquadricObj *unitCircle = 0;
  unitCircle = gluNewQuadric();
  gluDisk(unitCircle, 0, 1, 3, 1);
  gluDeleteQuadric(unitCircle);
}

/**
 * Zeichen-Funktion zur Erstellung des Hintergrundquadrats, um die Textur
 * darauf zu mappen.
 */
void drawBackgroundSquare(void)
{
  glPushMatrix();
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    {
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(-1.0f, 1.0f);

      glTexCoord2f(0.0f, 1.0f);
      glVertex2f(-1.0f, -1.0f);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(1.0f, -1.0f);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(1.0f, 1.0f);
    }
    glEnd();
  }
  glPopMatrix();
}

/**
 * Zeichen-Funktion fuer einen Kontrollpunkt.
 */
void drawCheckPoint(void)
{
  glPushMatrix();
  {
    glScalef(CHECKPOINT_RADIUS, CHECKPOINT_RADIUS, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    drawUnitCircle();
  }
  glPopMatrix();
}

/**
 * Zeichen-Funktion fuer alle Kontrollpunkte.
 */
void drawCheckpoints(void)
{
  int *currentLevel = getCurrentLevel();

  CGPoint2f *checkStart = getCheckpointStart();
  CGPoint2f *checkEnd = getCheckpointEnd();
  CGPoint2f *check1 = getCheckpoint1();
  CGPoint2f *check2 = getCheckpoint2();
  CGPoint2f *check3 = getCheckpoint3();
  CGPoint2f *check4 = getCheckpoint4();

  glPushMatrix();
  {
    glTranslatef((*checkStart)[0], (*checkStart)[1], 0.0f);
    drawCheckPoint();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef((*checkEnd)[0], (*checkEnd)[1], 0.0f);
    drawCheckPoint();
  }
  glPopMatrix();
  switch (*currentLevel)
  {
  case 3:
    glPushMatrix();
    {
      glTranslatef((*check4)[0], (*check4)[1], 0.0f);
      drawCheckPoint();
    }
    glPopMatrix();
  case 2:
    glPushMatrix();
    {
      glTranslatef((*check3)[0], (*check3)[1], 0.0f);
      drawCheckPoint();
    }
    glPopMatrix();
  case 1:
    glPushMatrix();
    {
      glTranslatef((*check2)[0], (*check2)[1], 0.0f);
      drawCheckPoint();
    }
    glPopMatrix();
    glPushMatrix();
    {
      glTranslatef((*check1)[0], (*check1)[1], 0.0f);
      drawCheckPoint();
    }
    glPopMatrix();
    break;
  default:
    break;
  }
}

/**
 * Zeichenfunktion der Rakete.
 */
void drawRocket(void)
{
  CGPoint2f *rocketCenter = getRocketCenter();
  glPushMatrix();
  {
    glTranslatef((*rocketCenter)[0], (*rocketCenter)[1], 0.0f);
    // Triebwerk gross
    glPushMatrix();
    {
      glColor3f(0.0f, 0.0f, 0.0f);
      glTranslatef(0.0f, -0.15f, 0.0f);
      glScalef(0.075f, 0.1f, 0.0f);
      drawUnitTriangle();
    }
    glPopMatrix();
    // Triebwerke klein
    glPushMatrix();
    {
      glColor3f(0.0f, 0.0f, 0.0f);
      glTranslatef(0.0f, -0.05f, 0.0f);
      glScalef(0.125f, 0.125f, 0.0f);
      drawUnitTriangle();
    }
    glPopMatrix();
    // Koerper
    glPushMatrix();
    {
      glColor3f(1.0f, 0.0f, 0.0f);
      glScalef(0.125f, 0.275f, 0.0f);
      drawUnitSquare();
    }
    glPopMatrix();
    // Spitze
    glPushMatrix();
    {
      glColor3f(1.0f, 0.0f, 0.0f);
      glTranslatef(0.0f, 0.175f, 0.0f);
      glScalef(0.0725f, 0.075f, 0.0f);
      drawUnitTriangle();
    }
    glPopMatrix();
    // Fenster
    glPushMatrix();
    {
      glColor3f(0.0f, 0.75f, 1.0f);
      glTranslatef(0.0f, 0.1f, 0.0f);
      glScalef(0.05f, 0.05f, 0.0f);
      drawUnitCircle();
    }
    glPopMatrix();
  }
  glPopMatrix();
}

/**
 * Zeichenfunktion fuer einen einzelnen Meteor.
 */
void drawMeteor(void)
{
  // Meteorfläche
  glPushMatrix();
  {
    glColor3f(0.7f, 0.7f, 0.7f);
    glScalef(0.175f, 0.175f, 1.0f);
    drawRoughUnitCircle();
  }
  glPopMatrix();
  // Krater
  glPushMatrix();
  {
    glColor3f(0.45f, 0.45f, 0.45f);
    glTranslatef(0.06f, 0.05f, 0.0f);
    glScalef(0.06f, 0.06f, 1.0f);
    drawUnitCircle();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glColor3f(0.45f, 0.45f, 0.45f);
    glTranslatef(-0.06f, -0.05f, 0.0f);
    glScalef(0.05f, 0.05f, 1.0f);
    drawUnitCircle();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glColor3f(0.45f, 0.45f, 0.45f);
    glTranslatef(0.06f, -0.1f, 0.0f);
    glScalef(0.02f, 0.02f, 1.0f);
    drawUnitCircle();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glColor3f(0.45f, 0.45f, 0.45f);
    glTranslatef(-0.07f, 0.07f, 0.0f);
    glScalef(0.02f, 0.02f, 1.0f);
    drawUnitCircle();
  }
  glPopMatrix();
}

/**
 * Zeichnen aller Meteoriten.
 */
void drawMeteors(void)
{
  int *currentLevel = getCurrentLevel();

  glPushMatrix();
  {
    glTranslatef(0.4f, 0.4f, 0.0f);
    glCallList(g_meteorListID);
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(-0.5f, -0.2f, 0.0f);
    glCallList(g_meteorListID);
  }
  glPopMatrix();

  switch (*currentLevel)
  {
  case 3:
    glPushMatrix();
    {
      glTranslatef(-0.2f, 0.5f, 0.0f);
      glCallList(g_meteorListID);
    }
    glPopMatrix();
  case 2:
    glPushMatrix();
    {
      glTranslatef(-0.7f, -0.7f, 0.0f);
      glCallList(g_meteorListID);
    }
    glPopMatrix();
  case 1:
    glPushMatrix();
    {
      glTranslatef(0.7f, -0.1f, 0.0f);
      glCallList(g_meteorListID);
    }
    glPopMatrix();
    glPushMatrix();
    {
      glTranslatef(0.2f, -0.6f, 0.0f);
      glCallList(g_meteorListID);
    }
    glPopMatrix();
    break;
  default:
    break;
  }
}

/**
 * Zeichenfunktion fuer die Normalen der Kurve.
 */
void drawNormals()
{
  Vertex *Vertices = getVertices();
  Normal *Normals = getNormals();
  int *numberOfVertices = getNumberOfVertices();
  int i = 0;

  for (i = 0; i < (*numberOfVertices); i++)
  {
    glPushMatrix();
    {
      glColor3f(1.0f, 0.0f, 0.0f);
      glBegin(GL_LINES);
      {
        glVertex3d(Normals[i][NX], Normals[i][NY], Normals[i][NZ]);
        glVertex3d(Vertices[i][CX], Vertices[i][CY], 0.0);
      }
      glEnd();
      glLineWidth(2);
    }
    glPopMatrix();
  }
}

/**
 * Zeichen-Funktion fuer den Hilfe Screen.
 */
void drawHelp(void)
{
  /* Textfarbe */
  GLfloat textColor[3] = {1.0f, 0.0f, 0.0f};

  /* Text ausgegeben */
  drawString(0.05f, 0.1f, textColor, "Tastenbelegung");
  drawString(0.05f, 0.2f, textColor, "q/Q/Esc : Beendet das Spiel.");
  drawString(0.05f, 0.25f, textColor, "h/H : Ruft diese Hilfeausgabe auf und pausiert das Spiel.");
  drawString(0.05f, 0.3f, textColor, "+/- : Kurvenstuecke erhoehen bzw. verringern");
  drawString(0.05f, 0.35f, textColor, "n/N : Kurvennormalen anzeigen/ausschalten");
  drawString(0.05f, 0.4f, textColor, "b/B : Wechseln zwischen Bezier- oder B-Spline-Kurve");
  drawString(0.05f, 0.45f, textColor, "s/S : Rakete starten");
  drawString(0.05f, 0.5f, textColor, "k/K : Bounding Volumen an- und ausschalten");
  drawString(0.05f, 0.55f, textColor, "c/C : Konvexe Huelle an- und ausschalten");
  drawString(0.05f, 0.6f, textColor, "F1 : Schaltet zwischen normalem und WireFrame/-Mode");
  drawString(0.05f, 0.65f, textColor, "F2 : Waehle zwischen Full-Frame und Window-Frame");
  drawString(0.05f, 0.7f, textColor, "F3 : Textur an- und ausschalten");
}

/**
 * Zeichen-Funktion fuer jeden Frame.
 * 
 * Setzt die Hintergrun-Textur, zeichnet das Vertex-Array (Kurve), die
 * Meteoriten, die Rakete, die Kontrollpunkte, die Normalen (wenn aktiviert) und die Hilfe-
 * ausgabe (wenn aktiviert).
 */
void drawScene(void)
{
  int *help = getHelp();
  int *hull = getHull();
  int *numberOfVertices = getNumberOfVertices();
  int *countOfHullIndices = getCountOfVertices();
  int *toggleNormals = getNormalsToggle();
  int *bezierToggle = getBezierToggle();
  Vertex *Vertices = getVertices();
  GLuint *Indices = getIndices();
  GLuint *HullIndices = getHullIndices();
  Normal *Normals = getNormals();

  // Zeichnen des Hintergrunds mit Textur
  if (getTexturingStatus())
  {
    /* Texturierung aktivieren */
    glEnable(GL_TEXTURE_2D);
  }
  bindTexture(texBackground);
  drawBackgroundSquare();

  /* Texturierung deaktivieren */
  glDisable(GL_TEXTURE_2D);

  glLineWidth(3);

  // Vertex Array definieren und bei Änderungen aktualisieren
  glVertexPointer(2,                   // Komponenten pro Vertex (x,y)
                  GL_DOUBLE,           // Typ der Komponenten
                  sizeof(Vertex),      // Offset zwischen 2 Vertizes im Array
                  &(Vertices[0][CX])); // Zeiger auf die 1. Komponente

  glNormalPointer(GL_DOUBLE, sizeof(Normal), &(Normals[0][NX]));

  glDrawElements(GL_LINE_STRIP,    // Primitivtyp
                 COUNT_OF_INDICES, // Anzahl Indizes zum Zeichnen
                 GL_UNSIGNED_INT,  // Typ der Indizes
                 Indices);         // Index Array

  if ((*hull))
  {
    // Vertex Array definieren und bei Änderungen aktualisieren
    glVertexPointer(2,                   // Komponenten pro Vertex (x,y)
                    GL_DOUBLE,           // Typ der Komponenten
                    sizeof(Vertex),      // Offset zwischen 2 Vertizes im Array
                    &(Vertices[0][HX])); // Zeiger auf die 1. Komponente
    
    glDrawElements(GL_POLYGON,            // Primitivtyp
                   (*countOfHullIndices), // Anzahl Indizes zum Zeichnen
                   GL_UNSIGNED_INT,       // Typ der Indizes
                   HullIndices);          // Index Array
  }

  glPopMatrix();

  drawMeteors();

  drawRocket();

  drawCheckpoints();

  if ((*toggleNormals))
  {
    if ((*bezierToggle) == 0)
    {
      calcSpline();
    }
    else
    {
      calcBezier();
    }
    drawNormals();
  }

  if ((*help))
  {
    drawHelp();
  }

  /* DEBUG-Ausgabe */
  // GLSTATE;
}

/**
 * Erzeugen der Display-Liste fuer Meteoriten.
 */
static void
initDisplayLists(void)
{
  /* Eine Display-Liste anfordern */
  g_meteorListID = glGenLists((GLsizei)1);
  if (g_meteorListID != 0)
  {
    /* Erste Diplay-Liste */
    glNewList(g_meteorListID, GL_COMPILE);
    drawMeteor();
    glEndList();
  }
  else
  {
    CG_ERROR(("Konnte Displaylisten nicht erzeugen\n"));
  }
}

/**
 * Loescht alle Displaylisten.
 */
void deleteDisplayLists(void)
{
  glDeleteLists(g_meteorListID, 1);
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

  glColor3f(1.0f, 1.0f, 1.0f);

  initDisplayLists();

  glEnableClientState(GL_VERTEX_ARRAY);

  initVertices();
  initIndices();
  initHullIndices(0);
  calcSpline();
  calcConvexHull();
  initRocketPosition();

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

/**
 * (De-)aktiviert den Normalen-Modus.
 */
void toggleNormals(void)
{
  /* Flag: Normals: ja/nein */
  static GLboolean normals = GL_FALSE;
  int *toggleNormals = getNormalsToggle();

  /* Modus wechseln */
  normals = !normals;

  if (normals)
  {
    (*toggleNormals) = 1;
  }
  else
  {
    (*toggleNormals) = 0;
  }
}