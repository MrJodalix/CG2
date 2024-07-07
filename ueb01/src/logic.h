#ifndef __LOGIC_H__
#define __LOGIC_H__

/**
 * @file
 * Schnittstelle des Logik-Moduls.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Position und Bewegung der Objekte im Spiel.
 * Die Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * @author Joshua-Scott Schoettke, Ilana Schmara (Gruppe 1).
 */

/* ---- Header einbinden ---- */
#include "types.h"

/**
 * Rechnet die Maus-Koordinaten in Bildschirmkoordinaten um.
 * @param x X-Koordinate des Mauszeigers
 * @param y Y-Koordinate des Mauszeigers
 * @param width Breite des Anzeigefensters
 * @param height Hoehe des Anzeigefensters
 */
void
calcMousePosition (int x, int y, float width, float height);

/**
 * Berechnet neue Position der Rakete.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void
calcRocketPosition (double interval);

/**
 * Berechnet die Koordinaten fuer die B-Spline-Kurve und fuegt diese in das Vertex-Array hinzu.
 */
void calcSpline();

/**
 * berechnet die konvexe Huelle
*/
void calcConvexHull();

/**
 * Berechnet die Koordinaten fuer die Bezier-Kurve und fuegt diese in das Vertex-Array hinzu.
 */
void calcBezier();

/**
 * Setzt die Rakete auf ihre Startposition (zurueck).
*/
void initRocketPosition(void);

/*
 * Erhöht die Anzahl der Vertices im Vertex-Array.
 */
void addVertices();

/*
 * Verringert die Anzahl der Vertices im Vertex-Array.
 */
void deleteVertices();

/**
 * Leert den Speicher für das Vertex-Array.
 */
void freeAll(void);

/**
 * Aendert den Wert für Pause auf den jeweils anderen Wert (0 oder 1).
 */
void
togglePause(void);

/**
 * Aendert den Toggle für Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void 
toggleHelp(void);

/**
 * Aendert den Toggle für Hilfe auf den jeweils anderen Wert (0 oder 1)
 */
void toggleHull(void);

/**
 * Liefert aktuelle Postion (des Mittelpunktes) aller Waende.
 * @return Postion (des Mittelpunktes) der Waende.
 * Aendert den Toggle fuer die Bezier-Kurve auf den jeweils anderen Wert (0 oder 1).
 */
void toggleBezier(void);

/**
 * Liefert aktuelle Postion (des Mittelpunktes) der Rakete.
 * @return Postion (des Mittelpunktes) der Rakete.
 */
CGPoint2f *getRocketCenter(void);

/**
 * Gibt den Status fuer Pause zurueck.
 */
int *getPause(void);

/*
 * Gibt den Status fuer die Hilfeausgabe zurueck.
 */
int *getHelp(void);

/*
 * Gibt den Status fuer die Huelle zurueck.
 */
int *getHull(void);

/**
 * Gibt den Status fuer den Bezier-Toggle zurueck.
*/
int *getBezierToggle(void);

/**
 * Liefert, den Index des aktuellen Levels.
 * @return den Index des aktuellen Levels.
 */
int *getCurrentLevel(void);


/**
 * Liefert aktuelle Postion des ersten Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpointStart(void);

/**
 * Liefert aktuelle Postion des letzten Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpointEnd(void);

/**
 * Liefert aktuelle Postion des 2. Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpoint1 (void);

/**
 * Liefert aktuelle Postion des 3. Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpoint2 (void);

/**
 * Liefert aktuelle Postion des 4. Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpoint3 (void);

/**
 * Liefert aktuelle Postion des 5. Kontrollpunkts.
 * @return Postion des Kontrollpunkts.
 */
CGPoint2f *
getCheckpoint4 (void);

/*
 * Gibt den Status fuer die Normalen zurueck.
 */
int *getNormalsToggle(void);

/**
 * Gibt die Anzahl aller Vertices zurueck
 */
int *getNumberOfVertices(void);

/**
 * Gibt die Anzahl aller Vertices zurueck
 */
int *getCountOfVertices(void);

/**
 * Initialisiert die dynamischen Arrays
 */
void initVertices(void);

/**
 * Initialisiert die Indices fuer das dynamischen Array
*/
void initIndices(void);

/**
 *
 */
void initHullIndices(int count);

/**
 * Liefert das Vertex-Array zurueck.
 * @return Das Vertex-Array.
 */
Vertex *
getVertices(void);

/**
 * Liefert das Normals-Array zurueck.
 * @return Das Normals-Array.
 */
Normal *
getNormals(void);

/**
 * Liefert das Indices-Array zurueck.
 * @return Das Indices-Array.
 */
GLuint *
getIndices(void);

/**
 * Liefert das Indices-Array zurueck.
 * @return Das Indices-Array.
 */
GLuint *
getHullIndices(void);

#endif
