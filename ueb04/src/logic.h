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
 * Berechnet die Bewegung der Partikel mittels Euler-Integration.
 * @param interval Das Zeitinterval zwischen den Aufrufen der Funktion
 */
void calcEulerIntegration(double interval);

/**
 * Reserviert den Speicher für das Partikel-Array.
 */
void initMemory(void);

/**
 * Initialisiert die Anfangswerte der Partikel.
 */
void initParticles(void);

/**
 * Leert den Speicher fuer das Partikel-Array.
 */
void freeAll(void);

/**
 * Aendert den Toggle fuer die Pause.
 */
void pauseToggle(void);

/**
 * Aendert den Toggle fuer Hilfe auf den jeweils anderen Wert (0 oder 1).
 */
void toggleHelp(void);

/**
 * Erhoeht die Anzahl der Partikel.
 */
void addParticle(void);

/**
 * Verringert die Anzahl der Partikel.
 */
void deleteParticle(void);

/*
 * Gibt den Status fuer die Hilfeausgabe zurueck.
 * @return den Status
 */
int *getHelp(void);

/*
 * Aendert den Switch fuer die Textur.
 */
void switchTexture(void);

/*
 * Gibt die aktuelle Textur zurueck.
 * @return die Textur
 */
int *getTexture(void);

/**
 * Gibt die Anzahl aller Partikel zurueck.
 * @return die Anzahl der Partikel
 */
int *getNumberOfParticles(void);

/*
 * Gibt die Position der roten Kugel zurück.
 * @return die Position der roten Kugel
 */
CGPoint3f *getRedBallPos(void);

/*
 * Gibt die Position der blauen Kugel zurück.
 * @return die Position der blauen Kugel
 */
CGPoint3f *getBlueBallPos(void);

/**
 * Liefert das Partikel-Array zurueck.
 * @return Das Partikel-Array.
 */
Vertex *
getParticles(void);

/**
 * Liefert den Status der Pause.
 * @return der Status
 */
int *getPause(void);

/*
 * Aendert das ausgewaehlte Partikel.
 */
void switchParticel(void);

/*
 * Liefert das ausgewaehlte Partikel zurueck.
 * @return das aktive Partikel
 */
int *getActiveParticle(void);

/**
 * Gibt die Lichtposition zurueck.
 * @return die Lichtposition
 */
CGPoint4f *getLightPos(void);

/**
 * Liefert den Partikel-Typ zurueck.
 * @return Den Partikel-Typ.
 */
int *getParticleType(void);

/**
 * Aendert den Partikel-Typ.
 */
void switchParticleType(void);

/**
 * Gibt die Kameraposition zurueck.
 * @return die Kameraposition
 */
int *getCamera(void);

/**
 * Aendert die Kamera.
 */
void switchCamera(void);

/**
 * Wechselt das Ziel der Partikel.
 */
void switchTarget(void);

#endif
