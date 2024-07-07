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
#define LEFT -1.0f
/** x-Koordinate rechter Rand */
#define RIGHT 1.0f
/** y-Koordinate oberer Rand */
#define GROUND -1.0f
/** y-Koordinate unterer Rand */
#define CEILING 1.0f
/** z-Koordinate hinterer Rand */
#define BACK -1.0f
/** z-Koordinate vorderer Rand */
#define FRONT 1.0f

/** Der Kugel Radius */
#define RADIUS 0.15f

/** Der Kugel Radius */
#define CUBE_R_RADIUS 0.2f
#define CUBE_R_X -0.6f
#define CUBE_R_Z 0.5f

/** Fensterbreite */
#define FRAMEBUFFER_WIDTH 800
/** Fensterhoehe */
#define FRAMEBUFFER_HEIGHT 600

/** Frostum near */
#define NEAR 0.1f
/** Frostum far */
#define FAR 100.0f

#define MATERIAL_DEFAULT_AMBIENT 0.3f 
#define MATERIAL_DEFAULT_DIFFUSE 1.0f 
#define MATERIAL_DEFAULT_SPECULAR 0.2f 
#define MATERIAL_DEFAULT_SHININESS 2
#define MATERIAL_DEFAULT_EMISSION 0.0f 


#define CX 0 
#define CY 1 
#define CZ 2 
#define DX 3
#define DY 4 
#define DZ 5 
#define cR 6
#define cG 7 
#define cB 8 
#define NX 9
#define NY 10 
#define NZ 11

/* ---- Typedeklarationen ---- */
/** Repraesentation eines Punktes durch Raycasting */
typedef GLfloat CGPoint12f[12];

/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

/** Punkt im 2D-Raum */
typedef GLint CGPoint2i[2];

/** Punkt im 3D-Raum */
typedef GLfloat CGPoint3f[3];

/** Vektor im 3D-Raum */
typedef GLfloat CGVector3f[3];

/** RGB-Farbwert */
typedef GLfloat CGColor3f[3];

/** Punkt im 3D-Raum (homogene Koordinaten) */
typedef GLfloat CGPoint4f[4];

/** Ausmasse eines Rechtecks (Breite/Hoehe) */
typedef GLint CGDimensions2i[2];

/** */
typedef struct
{
    CGPoint3f A;
    CGPoint3f B;
    CGPoint3f C;
} sTriangle;

#endif
