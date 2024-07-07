#ifndef __TEXTURE_H__
#define __TEXTURE_H__
/**
 * @file
 * Schnittstelle des Texturen-Moduls.
 * Das Modul kapselt die Textur-Funktionalitaet (insbesondere das Laden und
 * Binden) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Texturierung mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/**
 * Typ fuer Texturenbezeichner.
 */
typedef enum
{
  texBackground
} TexName;


/**
 * Laed Texturen und initialisiert das Texturmapping.
 * @return 1, wenn Laden und Initialisieren erfolgreich war, sonst 0.
 */
int initTextures (void);

/**
 * Bindet die Textur texture, so dass sie fuer alle nachfolgende gezeichneten
 * Primitiven verwendet wird.
 * @param texture Bezeichner der Textur, die gebunden werden soll.
 */
void bindTexture (TexName texture);

/**
 * Schaltet den Status der automatischen Texturierung um.
 */
void toggleAutomaticTextureCoordinates (void);

/**
 * Liefert den Status der Texturierung.
 * @return Status der Texturierung (an/aus).
 */
int getTexturingStatus (void);

/**
 * Setzt den Status der Texturierung.
 * @param status Status der Texturierung (an/aus).
 */
void setTexturingStatus (int status);



#endif
