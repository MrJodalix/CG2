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

// Displaylist-IDs
static GLuint g_checkPointID = 0;
static GLuint g_PointID = 1;

// Aenderung der Zeichengenauigkeit
int vertexCountChange = 0;

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
 * Zeichenfuntkion fuer die interpolierten Punkte.
 */
void drawPointSphere()
{
  /* Material - weiss */
  float matAmbient[] = {0.1, 0.1, 0.1, 1.0};
  float matDiffuse[] = {1.0, 1.0, 1.0, 1.0};
  float matSpecular[] = {0.1, 0.1, 0.1, 1.0};
  float matShininess = 1;

  // Setzen der Material-Parameter
  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);

  glPushMatrix();
  {
    glColor3f(0.0f, 0.0f, 1.0f);
    glScalef(0.05f, 0.05f, 0.05f);
    drawUnitSphere();
  }
  glPopMatrix();
}

/**
 * Zeichenfunktion aller interpolierten Punkte.
 */
void drawCheckPointSpheres(void)
{

  float *geometryMat = getGeometryMat();
  int *numberOfCheckPoints = getNumberOfCheckPoints();
  int i = 0, j = 0;
  double coordX = XSTARTCHECKPOINT, coordZ = ZSTARTCHECKPOINT;
  int *actualCheckPoint = getActualCheckPoint();

  /* Material */
  float matAmbient[] = {1.0, 1.0, 1.0, 1.0};
  float matDiffuse[] = {0.5, 0.5, 0.5, 1.0};
  float matSpecular[] = {0.1, 0.1, 0.1, 1.0};
  float matShininess = 1;

  /* Setzen der Material-Parameter */
  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);

  for (j = 0; j < (*numberOfCheckPoints); j += sqrt(*numberOfCheckPoints))
  {
    for (i = 0; i < sqrt(*numberOfCheckPoints); i++)
    {
      glPushMatrix();
      {
        if (i + j != (*actualCheckPoint))
        {
          glColor3f(1.0f, 1.0f, 1.0f);
        }
        else
        {
          glColor3f(1.0f, 0.0f, 0.0f);
        }
        glTranslatef(coordX, geometryMat[i + j], coordZ);
        glScalef(0.02f, 0.02f, 0.02f);
        drawUnitSphere();
      }
      glPopMatrix();
      coordX += (((-XSTARTCHECKPOINT) + XENDCHECKPOINT) / (sqrt(*numberOfCheckPoints) - 1.0));
    }
    coordZ += (((-ZSTARTCHECKPOINT) + ZENDCHECKPOINT) / (sqrt(*numberOfCheckPoints) - 1.0));
    coordX = XSTARTCHECKPOINT;
  }
}

/**
 * Zeichenfunktion aller interpolierten Punkte.
 */
void drawInterpolatePointSpheres(void)
{
  Vertex *Vertices = getVertices();
  int *numberOfInterpolatePoints = getNumberOfInterPoints();
  int i = 0;

  /* Material */
  float matAmbient[] = {1.0, 1.0, 1.0, 1.0};
  float matDiffuse[] = {0.5, 0.5, 0.5, 1.0};
  float matSpecular[] = {0.1, 0.1, 0.1, 1.0};
  float matShininess = 1;

  /* Setzen der Material-Parameter */
  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);

  for (i = 0; i < (*numberOfInterpolatePoints); i++)
  {
    glPushMatrix();
    {
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(Vertices[i][CX], Vertices[i][CY], Vertices[i][CZ]);
      glScalef(0.01f, 0.01f, 0.01f);
      drawUnitSphere();
    }
    glPopMatrix();
  }
}

/**
 *
 *
 */
void drawLight(float *lightPos)
{
  /* Material */
  float matAmbient[] = {0.5, 0.5, 0.5, 1.0};
  float matDiffuse[] = {0.3, 0.3, 0.3, 1.0};
  float matSpecular[] = {0.2, 0.2, 0.2, 1.0};
  float matShininess = 0.5;
  float matEmission[] = {0.1, 0.1, 0.1, 1.0};

  /* Setzen der Material-Parameter */
  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);

  glPushMatrix();
  {
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
    glScalef(0.05f, 0.05f, 0.05f);
    drawUnitSphere();
  }
  glPopMatrix();
}

/**
 * Zeichenfunktion fuer die Normalen.
 */
void drawNormals()
{
  Normal *Normals = getNormals();
  Vertex *Vertices = getVertices();
  int *numberOfInterPoints = getNumberOfInterPoints();

  /* Material */
  float matAmbient[] = {1.0, 1.0, 1.0, 1.0};
  float matDiffuse[] = {1.0, 1.0, 1.0, 1.0};
  float matSpecular[] = {0.1, 0.1, 0.1, 1.0};
  float matShininess = 1;

  /* Setzen der Material-Parameter */
  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, &matShininess);

  for (int i = 0; i < (*numberOfInterPoints); i++)
  {
    glPushMatrix();
    {
      glColor3f(1.0f, 1.0f, 1.0f);
      glLineWidth(2.0);
      glTranslatef(Vertices[i][CX], Vertices[i][CY], Vertices[i][CZ]);
      glScalef(0.3f, 0.3f, 0.3f);
      glBegin(GL_LINES);
      {
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(Normals[i][NX], Normals[i][NY], Normals[i][NZ]);
      }
      glEnd();
    }
    glPopMatrix();
  }
}

/**
 * Zeichen-Funktion fuer den Hilfe-Screen.
 */
void drawHelp(void)
{
  /* Textfarbe */
  GLfloat textColor[3] = {1.0f, 0.0f, 0.0f};

  /* Text ausgegeben */
  drawString(0.05f, 0.1f , textColor, "Tastenbelegung");
  drawString(0.05f, 0.2f , textColor, "q/Q/Esc : Beendet das Spiel.");
  drawString(0.05f, 0.25f, textColor, "h/H : Ruft diese Hilfeausgabe auf und pausiert das Spiel.");
  drawString(0.05f, 0.3f , textColor, "+/- : Genauigkeit der Splineflaeche anpassen");
  drawString(0.05f, 0.35f, textColor, "p/P : Flaechenteile ergaenzen");
  drawString(0.05f, 0.4f , textColor, "o/O : Flaechenteile entfernen");
  drawString(0.05f, 0.45f, textColor, "t/T : Textur an- und ausschalten");
  drawString(0.05f, 0.5f , textColor, "z/Z : Textur wechseln");
  drawString(0.05f, 0.55f, textColor, "c/C : Kameraflug starten");
  drawString(0.05f, 0.6f , textColor, "Pfeiltasten links/rechts : Kontrollpunkt wechseln");
  drawString(0.05f, 0.65f, textColor, "Pfeiltasten oben/unten : Kontrollpunkt-Hoehe anpassen");
  drawString(0.05f, 0.7f , textColor, "F1 : Schaltet zwischen normalem und WireFrame/-Mode");
  drawString(0.05f, 0.75f, textColor, "F2 : Waehle zwischen Full-Frame und Window-Frame");
  drawString(0.05f, 0.8f , textColor, "F3 : Normalen an- und ausschalten");
  drawString(0.05f, 0.85f, textColor, "F4 : Interpolierte Punkte ein- und ausblenden");
  drawString(0.05f, 0.9f , textColor, "F5 : Kontrollpunkte ein- und ausblenden");
}

/**
 * Zeichen-Funktion fuer jeden Frame.
 */
void drawScene(void)
{
  int *help = getHelp();
  int *numberOfInterPoints = getNumberOfInterPoints();
  int *toggleNormals = getNormalsToggle();
  GLuint *Indices = getIndices();
  Vertex *Vertices = getVertices();
  Normal *Normals = getNormals();

  /* Lichtberechnung aktivieren */
  glEnable(GL_LIGHTING);

  /* Lichtquelle */
  const float distance = 1.0f;
  CGPoint4f lightPos;
  lightPos[0] = distance * sinf((float)glutGet(GLUT_ELAPSED_TIME) / 1000);
  lightPos[1] = 0.5f;
  lightPos[2] = distance * cosf((float)glutGet(GLUT_ELAPSED_TIME) / 1000);
  lightPos[3] = 1.0;

  /* Position der Lichtquelle setzen */
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  /* Erste Lichtquelle aktivieren */
  glEnable(GL_LIGHT0);

  // Vertex Array definieren und bei Aenderungen aktualisieren
  if (vertexCountChange != (*numberOfInterPoints))
  {
    if (vertexCountChange != 0)
    {
      freeAll();
    }

    calcSplinePlane();

    Indices = getIndices();
    Vertices = getVertices();
    Normals = getNormals();

    // Vertex und Color Array definieren und bei Änderungen aktualisieren
    glVertexPointer(3,                   // Komponenten pro Vertex (x,y,z)
                    GL_DOUBLE,           // Typ der Komponenten
                    sizeof(Vertex),      // Offset zwischen 2 Vertizes im Array
                    &(Vertices[0][CX])); // Zeiger auf die 1. Komponente

    glColorPointer(3, GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CR]));

    glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex), &(Vertices[0][CS]));

    glNormalPointer(GL_DOUBLE, sizeof(Normal), &(Normals[0][NX]));

    vertexCountChange = (*numberOfInterPoints);
  }

  if ((*getCheckPoint()))
  {
      glEnable(GL_COLOR_MATERIAL);
      drawCheckPointSpheres();
      glDisable(GL_COLOR_MATERIAL);
  }

  // Zeichnen der Textur der Flaeche
  if (getTexturingStatus())
  {
    /* Texturierung aktivieren */
    glEnable(GL_TEXTURE_2D);
    switch ((*getSwitchTexture()))
    {
    case 0:
      bindTexture(texLandscape0);
      break;
    case 1:
      bindTexture(texLandscape1);
      break;
    case 2:
      bindTexture(texLandscape2);
      break;
    case 3:
      bindTexture(texLandscape3);
      break;
    default:
      bindTexture(texLandscape0);
      break;
    }
  }

  glEnable(GL_COLOR_MATERIAL);

  // Vertexarray Zeichnen in jedem Frame
  glDrawElements(GL_TRIANGLES,     // Primitivtyp
                 COUNT_OF_INDICES, // Anzahl Indizes zum Zeichnen
                 GL_UNSIGNED_INT,  // Typ der Indizes
                 Indices);         // Index Array

  glDisable(GL_COLOR_MATERIAL);

  /* Texturierung deaktivieren */
  glDisable(GL_TEXTURE_2D);

  if ((*getInterpolationPoint()))
  {
    glEnable(GL_COLOR_MATERIAL);
    drawInterpolatePointSpheres();
    glDisable(GL_COLOR_MATERIAL);
  }

  if ((*toggleNormals))
  {
    glEnable(GL_COLOR_MATERIAL);
    drawNormals();
    glDisable(GL_COLOR_MATERIAL);
  }

  glPushMatrix();
  {
    glEnable(GL_COLOR_MATERIAL);
    drawLight(lightPos);
    glDisable(GL_COLOR_MATERIAL);
  }
  glPopMatrix();

  if ((*help))
  {
    drawHelp();
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
}

/**
 * Erzeugen der Display-Liste fuer Kontrollunkte.
 */
static void
initDisplayLists(void)
{
  /* Zwei Display-Listen anfordern */
  g_checkPointID = glGenLists((GLsizei)2);
  if (g_checkPointID != 0)
  {
    /* Diplay-Liste fuer Kontrollpunkte */
    glNewList(g_checkPointID, GL_COMPILE);
    drawCheckPointSpheres();
    glEndList();

    /* Diplay-Liste fuer interpolierte Punkte */
    glNewList(g_PointID, GL_COMPILE);
    drawPointSphere();
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
  glDeleteLists(g_checkPointID, 2);
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

  initDisplayLists();

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  initVertices();
  calcSplinePlane();

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