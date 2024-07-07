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
 * initialisiert die beiden Structs mit den Vertices
 */
void initQuadStruct(void);

/**
 * Schleife ueber alle Framebufferpixel
 */
void raycasting(void);

/**
 * Schreibt die Hilfeausgabe auf das Terminal auf stdout.
 */
void drawHelp(void);

/*
 * Gibt die Position der blauen Kugel zurueck.
 */
CGPoint3f *getBlueBallPos(void);

/*
 * Gibt die Position des roten Wuerfels zurueck.
 */
CGPoint3f *getRedCubePos(void);

/*
 * Gibt die Position des gruenen Models zurueck.
 */
CGPoint3f *getGreenModelPos(void);

/**
 * gibt die Lichtposition zurueck
*/
CGPoint4f *getLightPos(int idx);

/**
 * schaltet die Lichter an und ab
*/
void toggleLight(int idx);

/**
 * schaltet die Lichter an und ab
*/
int getLightActive(int idx);

void toggleDrawBlue(void);

int getDrawBlue(void);

void toggleDrawRed(void);

int getDrawRed(void);

#endif
