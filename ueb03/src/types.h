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

/** Radius Kontrollpunkt */
#define CHECKPOINT_RADIUS (0.05f)

/** Gravitation */
#define GRAVITY (-0.981)

/** Masse einer Kugel */
#define MASS (0.2)

/** Radius der Spielkugeln*/
#define RADIUS (0.020f)

#define FRICTION (0.9)

#define SPRINGCONSTANT (5.0f)

#define LOWER (0.5f)

#define WALL_SCALING (0.05f)

#define LIGHT_DISTANCE (0.5f)

/** x-Koordinate linker Rand */
#define XSTARTCHECKPOINT -1.0
/** x-Koordinate rechter Rand */
#define XENDCHECKPOINT 1.0
/** y-Koordinate oberer Rand */
#define ZSTARTCHECKPOINT -1.0
/** y-Koordinate unterer Rand */
#define ZENDCHECKPOINT 1.0
/** x-Koordinate linker Rand */
#define XSTARTINTPOINT -0.5
/** x-Koordinate rechter Rand */
#define XENDINTPOINT 0.5
/** y-Koordinate oberer Rand */
#define ZSTARTINTPOINT -0.5
/** y-Koordinate unterer Rand */
#define ZENDINTPOINT 0.5

/** Anzahl an Indizes fuer das Vertex-Array */
#define COUNT_OF_INDICES ((sqrt(*numberOfInterPoints) - 1.0f) * ((sqrt(*numberOfInterPoints) - 1.0f) * 2) * 3)

/** Komponenten des Vertex-Arrays */
#define CX (0)
#define CY (1)
#define CZ (2)
#define CR (3)
#define CG (4)
#define CB (5)
#define CS (6)
#define CT (7)

/** Komponenten des Normals-Arrays */
#define NX (0)
#define NY (1)
#define NZ (2)


/* ---- Typedeklarationen ---- */

/** Datentyp fuer Vertex-Array */
typedef GLdouble Vertex[8];

/** Datentyp fuer Normal-Array */
typedef GLdouble Normal[3];


/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

/** Punkt im 2D-Raum */
typedef GLint CGPoint2i[2];

/** Punkt im 3D-Raum */
typedef GLfloat CGPoint3f[3];

/** Punkt im 3D-Raum mit Zusatzdaten wie Normale*/
typedef GLfloat CGPoint8f[8];

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
