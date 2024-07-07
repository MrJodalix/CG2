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
 * Berechnung der Spline-Flaeche.
 */
void calcSplinePlane(void);

/**
 * Initialisiert die Anfangspositionen der Kugeln.
 */
void initBallPosition(void);

/**
 * Initialisiert die Anfangspositionen der Waende.
 */
void initWallPosition(void);

/**
 * Berechnet die Bewegung einer rollenden Kugel mittels Euler-Integration.
*/
void calcEulerIntegration(double interval);

/**
 * Berechnung der Lichtposition
*/
void calcLightPos(double interval);

/*
 * Erhöht die Anzahl der Vertices im Vertex-Array.
 */
void addVertices();

/*
 * Verringert die Anzahl der Vertices im Vertex-Array.
 */
void deleteVertices();

/**
 * Aendert den Toggle fuer Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void switchStart(void);

/**
 * Wechselt zwischen den steuerbaren Waenden
 * @param idx Index der Wand
 */
void switchWall(int idx);

/**
 * Veraendert die Position der ausgewaehlten Wand
 * @param direction 0 Oben / 1 Rechts / 2 Unten / 3 Links
 */
void moveWall(int direction);

/**
 * Leert den Speicher fuer das Vertex-Array.
 */
void freeAll(void);

/**
 * Wechselt den aktuellen Checkpoint.
 * 
 * @param leftOrRight 0 ist links / != 0 ist rechts
 */
void switchCheckPoint(int leftOrRight);

/**
 * Aendert die Hoehe fuer die Kontrollpunkte.
 *
 * @param upOrDown 0 ist oben / != 0 ist unten
 */
void switchHeight(int upOrDown);
/**
 * Aendert den Toggle fuer Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void switchStart(void);

/**
 * gibt die Position der Kugel zurueck
 * @param count der Kugelindex
*/
CGPoint3f *getBall(int count);

/**
 * gibt die Position der Waende zurueck
 * @param count der Wandindex
*/
CGPoint3f *getWall(int count);

/**
 * Andert den Toggle fuer die Kontrollpunkteanzeige.
 */
void pauseToggle(void);

/**
 * Aendert den Toggle fuer Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void toggleHelp(void);

/*
 * Gibt den Status fuer die Hilfeausgabe zurueck.
 */
int *getHelp(void);

/*
 * Gibt den Status fuer die Hilfeausgabe zurueck.
 */
int *getSelectWall(void);

/**
 * Getter fuer den Startder Animation.
 */
int *getStart(void);

/*
 * Gibt die Lichtposition zurück.
 */
CGPoint4f *getLightPos(void);

/*
 * Gibt die Position der Kugel zurück.
 */
CGPoint8f *getBallPos(void);

/**
 * Andert den Toggle fuer die Kontrollpunkteanzeige
 */
void checkPointToggle();

/**
 * Getter fuer den Toggle fuer die Kontrollpunkteanzeige.
 */
int *getCheckPoint(void);

/**
 * Aendert den Toggle fuer die interpolierten Punkte.
 */
void interpolatePointToggle(void);

/**
 * Getter fuer den Toggle fuer die interpolierten Punkte.
 */
int *getInterpolationPoint(void);

/*
 * Aendert den Switch fuer die Textur.
 */
void switchTexture(void);

/*
 * Gibt den Switch fuer die Textur zurueck.
 */
int *getSwitchTexture(void);

/*
 * Gibt den Status fuer die Normalen zurueck.
 */
int *getNormalsToggle(void);

/**
 * Gibt die Anzahl aller Kontrollpunkte zurueck
 */
int *getNumberOfCheckPoints(void);

/**
 * Gibt die Anzahl aller Vertices zurueck
 */
int *getNumberOfInterPoints(void);

/**
 * Initialisiert die dynamischen Arrays
 */
void initVertices(void);


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
 * Liefert den ersten Wert der Geometrie Matrix.
 * @return die Geometrie-Matrix
 */
float *
getGeometryMat(void);

/**
 * @return Liefert den aktuellen Kontrollpunkt, der veraendert werden kann.
 */
int *
getActualCheckPoint(void);

/**
 * @return Liefert den aktuellen Kontrollpunkt, der veraendert werden kann.
 */
int *getPause(void);

#endif