/**
 * @file
 * Texturen-Modul.
 * Das Modul kapselt die Textur-Funktionalitaet (insbesondere das Laden und
 * Binden) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Texturierung mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "texture.h"
#include "debugGL.h"

/* Bibliothek um Bilddateien zu laden. Es handelt sich um eine
 * Bibliothek, die sowohl den Header als auch die Quelle in einer Datei
 * bereitstellt. Die Quelle kann durch die Definition des Makros
 * STB_IMAGE_IMPLEMENTATION eingebunden werden. Genauere Informationen
 * sind der "stb_image.h" zu entnehmen.
 *
 * Quelle: https://github.com/nothings/stb */
// define global functions as static inline
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h> // Bibliothek um Bilder zu laden
                           // Quelle: https://github.com/nothings/stb

/* ---- Typen ---- */
typedef struct
{
  GLuint id;
  char *filename;
} Texture;

/* ---- Konstanten ---- */
/** Anzahl der Texturen. */
#define TEX_COUNT (5)

/* ---- Globale Daten ---- */
static int g_texturingStatus = 1;

static Texture g_textures[TEX_COUNT];

/* ---- Funktionen ---- */

int getTexturingStatus(void)
{
  return g_texturingStatus;
}

void setTexturingStatus(int status)
{
  g_texturingStatus = status;
}

static int
initTextureArray(void)
{
  int i = 0;
  GLuint texIDs[TEX_COUNT];
  glGenTextures (TEX_COUNT, texIDs);

  for (i = 0; i < TEX_COUNT; i++)
  {
    g_textures[i].id = texIDs[i];
  }

  g_textures[texLandscape0].filename = "../content/textures/landscape_0.jpg";
  g_textures[texLandscape1].filename = "../content/textures/landscape_1.jpg";
  g_textures[texLandscape2].filename = "../content/textures/landscape_2.jpg";
  g_textures[texLandscape3].filename = "../content/textures/landscape_3.jpg";
  g_textures[texSky].filename = "../content/textures/sky.png";

  return (GLGETERROR == GL_NO_ERROR);
}

void bindTexture(TexName texture)
{
  glBindTexture(GL_TEXTURE_2D, g_textures[texture].id);
}

static unsigned int
calculateGLBitmapMode(int n)
{
  switch (n)
  {
  case 1:
    return GL_LUMINANCE;
  case 2:
    return GL_LUMINANCE_ALPHA;
  case 3:
    return GL_RGB;
  case 4:
    return GL_RGBA;
  default:
    return GL_RGB;
  }
}

static int
loadTextures(void)
{
  int i;

  unsigned char *data;
  int width, height;
  int channels;

  if (initTextureArray())
  {

    /* Alle Texturen nacheinander laden. */
    for (i = 0; i < TEX_COUNT; i++)
    {
      data = stbi_load(g_textures[i].filename, &width, &height, &channels, 0);

      if (data != NULL)
      {
        glBindTexture(GL_TEXTURE_2D, g_textures[i].id);

        gluBuild2DMipmaps(GL_TEXTURE_2D,
                          channels,
                          width,
                          height,
                          calculateGLBitmapMode(channels),
                          GL_UNSIGNED_BYTE, data);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        stbi_image_free(data);
      }
      else
      {
        INFO(("Textur %s konnte nicht geladen werden!\n", g_textures[i].filename));
        return 0;
      }
    }

    /* Alles in Ordnung? */
    return (GLGETERROR == GL_NO_ERROR);
  }
  else
  {
    INFO(("Erzeugen des Textur-Arrays fehlgeschlagen!\n"));
    return 0;
  }
}

void toggleAutomaticTextureCoordinates(void)
{
  static int automatic = GL_FALSE;

  automatic = !automatic;

  INFO(("Status der automatischen Texturierung: %d\n", automatic));

  if (automatic)
  {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
  }
  else
  {
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
  }
}

int initTextures(void)
{
  /* Ebenen-Koeffizienten für Generierung von Textur-Koordinaten */
  /* Parallele zur y-z-Ebene */
  GLfloat map_s[4] = {2.0f, 0.0f, 0.0f, 0.0f};
  /* Parallele zur x-z-Ebene */
  GLfloat map_t[4] = {0.0f, 2.0f, 0.0f, 0.0f};

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  /* Einstellungen für die automatische Generierung von Texturkoordinaten
   * nur gültig im Falle von impliziter Texturierung */
  glTexGenfv(GL_S, GL_OBJECT_PLANE, map_s);
  glTexGenfv(GL_T, GL_OBJECT_PLANE, map_t);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  

  /* Texturen laden */
  return loadTextures();
}
