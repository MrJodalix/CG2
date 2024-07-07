#ifndef __TYPES_H__
#define __TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
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
#include <GL/gl.h>
#endif

/* ---- Konstanten ---- */

/** x-Koordinate linker Rand */
#define XSTARTINTPOINT -1.0
/** x-Koordinate rechter Rand */
#define XENDINTPOINT 1.0
/** y-Koordinate oberer Rand */
#define ZSTARTINTPOINT -1.0
/** y-Koordinate unterer Rand */
#define ZENDINTPOINT 1.0
// Gausz Konstante
#define GAUSZ 40
// Geschwindigkeitsmittelkonstante
#define KV 1.0
// Reaktionsgeschwindigkeitskonstanten
#define KWEAK 5.0
//Eulersche Zahl
#define EULER exp(1.0)

/** Komponenten des Partikel-Arrays */
//Position
#define CX (0)
#define CY (1)
#define CZ (2)
//UP-Vektor
#define OX (3)
#define OY (4)
#define OZ (5)
//Geschwindigkeit
#define VX (6)
#define VY (7)
#define VZ (8)

/* ---- Typedeklarationen ---- */

/** Datentyp fuer Vertex-Array */
typedef GLdouble Vertex[9];

/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

/** Punkt im 2D-Raum */
typedef GLint CGPoint2i[2];

/** Punkt im 3D-Raum */
typedef GLfloat CGPoint3f[3];

/** Vektor im 3D-Raum */
typedef GLfloat CGVector3f[3];

/** Punkt im 3D-Raum (homogene Koordinaten) */
typedef GLfloat CGPoint4f[4];

/** Ausmasse eines Rechtecks (Breite/Hoehe) */
typedef GLint CGDimensions2i[2];

/** RGB-Farbwert */
typedef GLfloat CGColor3f[3];

/** Bewegungsrichtungen. */
enum e_Direction
{
    dirLeft,
    dirRight,
    dirUp,
    dirDown
};

/** Datentyp fuer Bewegungsrichtungen. */
typedef enum e_Direction CGDirection;

/** Mausereignisse. */
enum e_MouseEventType
{ mouseButton, mouseMotion, mousePassiveMotion };

/** Datentyp fuer Mausereignisse. */
typedef enum e_MouseEventType CGMouseEventType;

#endif
