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
#define BORDER_LEFT        (-0.9f)
/** x-Koordinate rechter Rand */
#define BORDER_RIGHT        (0.9f)
/** Radius Kontrollpunkt */
#define CHECKPOINT_RADIUS  (0.05f)
/** Radius Kontrollpunkt */
#define EMPTY_FIELD  (2.0f)

/** Anzahl an Indizes fuer das Vertex-Array */
#define COUNT_OF_INDICES ((*numberOfVertices))

/** Komponenten des Vertex-Arrays */
#define T  (0)
#define CX (1)
#define CY (2)
#define HX (3)
#define HY (4)

/** Komponenten des Normals-Arrays */
#define NX (0)
#define NY (1)
#define NZ (2)

/* ---- Typedeklarationen ---- */

/** Datentyp fuer Vertex-Array */
typedef GLdouble Vertex[5];

/** Datentyp fuer Normal-Array */
typedef GLdouble Normal[3];

/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

/** Punkt im 2D-Raum */
typedef GLint CGPoint2i[2];

/** Ausmasse eines Rechtecks (Breite/Hoehe) */
typedef GLint CGDimensions2i[2];

/** Mausereignisse. */
enum e_MouseEventType
{ mouseButton, mouseMotion, mousePassiveMotion };

/** Datentyp fuer Mausereignisse. */
typedef enum e_MouseEventType CGMouseEventType;

#endif
