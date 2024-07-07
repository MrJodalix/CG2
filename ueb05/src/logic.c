/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Position und Bewegung der Objekte im Spiel.
 * Die Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * @author Joshua-Scott Schoettke, Ilana Schmara (Gruppe 1).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"

/* ---- Globale Daten ---- */

CGPoint3f eyePos = {FRAMEBUFFER_WIDTH / 2.0f, FRAMEBUFFER_HEIGHT / 2.0f, 0.0f};

CGPoint4f lightPos0 = {0.8f, 0.8f, -0.3f, 1.0f};
CGPoint4f lightPos1 = {0.6f, 0.5f, 0.9f, 1.0f};

CGPoint3f blueBallPosition = {0.5f, GROUND + RADIUS, 0.0f};
CGPoint3f redCubePos = {CUBE_R_X, GROUND + CUBE_R_RADIUS, CUBE_R_Z};
CGPoint3f greenModelPosition = {-0.05f, GROUND + RADIUS, -0.7f};

CGPoint3f L_G_B_Point = {LEFT, GROUND, BACK};
CGPoint3f R_G_B_Point = {RIGHT, GROUND, BACK};
CGPoint3f L_C_B_Point = {LEFT, CEILING, BACK};
CGPoint3f R_C_B_Point = {RIGHT, CEILING, BACK};
CGPoint3f L_G_F_Point = {LEFT, GROUND, FRONT};
CGPoint3f R_G_F_Point = {RIGHT, GROUND, FRONT};
CGPoint3f L_C_F_Point = {LEFT, CEILING, FRONT};
CGPoint3f R_C_F_Point = {RIGHT, CEILING, FRONT};

CGPoint3f L_G_B_Quad = {CUBE_R_X - CUBE_R_RADIUS + 0.5, GROUND, CUBE_R_Z - CUBE_R_RADIUS - 0.5};
CGPoint3f R_G_B_Quad = {CUBE_R_X + CUBE_R_RADIUS + 0.5, GROUND, CUBE_R_Z - CUBE_R_RADIUS + 0.5};
CGPoint3f L_C_B_Quad = {CUBE_R_X - CUBE_R_RADIUS + 0.5, GROUND + CUBE_R_RADIUS * 2, CUBE_R_Z - CUBE_R_RADIUS - 0.5};
CGPoint3f R_C_B_Quad = {CUBE_R_X + CUBE_R_RADIUS + 0.5, GROUND + CUBE_R_RADIUS * 2, CUBE_R_Z - CUBE_R_RADIUS + 0.5};
CGPoint3f L_G_F_Quad = {CUBE_R_X - CUBE_R_RADIUS - 0.5, GROUND, CUBE_R_Z + CUBE_R_RADIUS - 0.5};
CGPoint3f R_G_F_Quad = {CUBE_R_X + CUBE_R_RADIUS - 0.5, GROUND, CUBE_R_Z + CUBE_R_RADIUS + 0.5};
CGPoint3f L_C_F_Quad = {CUBE_R_X - CUBE_R_RADIUS - 0.5, GROUND + CUBE_R_RADIUS * 2, CUBE_R_Z + CUBE_R_RADIUS - 0.5};
CGPoint3f R_C_F_Quad = {CUBE_R_X + CUBE_R_RADIUS - 0.5, GROUND + CUBE_R_RADIUS * 2, CUBE_R_Z + CUBE_R_RADIUS + 0.5};

sTriangle mainCube[12];
sTriangle redCube[12];

int drawBlue = 1;
int drawRed = 1;

int activeLight0 = 1;
int activeLight1 = 1;


/* ---- Lokale Funktionen ---- */

/**
 * Berechnet die Lenge aus eines Vektors
 * @param des die Ergebnislenge
 * @param a der Vektor
 */
void vecLength(float *des, CGVector3f a)
{
  (*des) = sqrt(pow(a[0], 2.0) + pow(a[1], 2.0) + pow(a[2], 2.0));
}

/**
 * Berechnet das Skalarprodukt aus zwei Vektoren
 * @param des die Skalarprodukt
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void dot(float *des, CGVector3f a, CGVector3f b)
{
  (*des) = (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

/**
 * Berechnet das Kreuzprodukt aus zwei Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void cross(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[1] * b[2] - a[2] * b[1];
  (*des)[1] = a[2] * b[0] - a[0] * b[2];
  (*des)[2] = a[0] * b[1] - a[1] * b[0];
}

/**
 * Berechnet das Kreuzprodukt aus zwei Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecAbs(CGVector3f *des)
{
  (*des)[0] = (*des)[0] < 0 ? (*des)[0] * -1 : (*des)[0];
  (*des)[1] = (*des)[1] < 0 ? (*des)[1] * -1 : (*des)[1];
  (*des)[2] = (*des)[2] < 0 ? (*des)[2] * -1 : (*des)[2];
}

/**
 * Setzt einen Vektor komponentenweise ein
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b Kommazahl
 */
void setVecf(CGVector3f *des, float a)
{
  (*des)[0] = a;
  (*des)[1] = a;
  (*des)[2] = a;
}

/**
 * Setzt einen Vektor komponentenweise ein
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void setVecV(CGVector3f *des, CGVector3f a)
{
  (*des)[0] = a[0];
  (*des)[1] = a[1];
  (*des)[2] = a[2];
}

/**
 * Berechnet die Addition zweier Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecAddVec(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[0] + b[0];
  (*des)[1] = a[1] + b[1];
  (*des)[2] = a[2] + b[2];
}

/**
 * Berechnet die Subtraktion zweier Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecSubVec(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[0] - b[0];
  (*des)[1] = a[1] - b[1];
  (*des)[2] = a[2] - b[2];
}

/**
 * Berechnet die Multiplikation zweier Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecMulVec(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[0] * b[0];
  (*des)[1] = a[1] * b[1];
  (*des)[2] = a[2] * b[2];
}

/**
 * Berechnet die Division zweier Vektoren
 * @param des der Ergebnisvektor
 * @param a erster Vektor
 * @param b zweiter Vektor
 */
void vecDivVec(CGVector3f *des, CGVector3f a, CGVector3f b)
{
  (*des)[0] = a[0] / b[0];
  (*des)[1] = a[1] / b[1];
  (*des)[2] = a[2] / b[2];
}

/**
 * Berechnet die Addition eines Vektors und eines Skalars
 * @param des der Ergebnisvektor
 * @param a Vektor
 * @param b Skalar
 */
void vecAddSca(CGVector3f *des, CGVector3f a, float b)
{
  (*des)[0] = a[0] + b;
  (*des)[1] = a[1] + b;
  (*des)[2] = a[2] + b;
}

/**
 * Berechnet die Subtraktion eines Vektors und eines Skalars
 * @param des der Ergebnisvektor
 * @param a Vektor
 * @param b Skalar
 */
void vecSubSca(CGVector3f *des, CGVector3f a, float b)
{
  (*des)[0] = a[0] - b;
  (*des)[1] = a[1] - b;
  (*des)[2] = a[2] - b;
}

/**
 * Berechnet die Multiplikation eines Vektors und eines Skalars
 * @param des der Ergebnisvektor
 * @param a Vektor
 * @param b Skalar
 */
void vecMulSca(CGVector3f *des, CGVector3f a, float b)
{
  (*des)[0] = a[0] * b;
  (*des)[1] = a[1] * b;
  (*des)[2] = a[2] * b;
}

/**
 * Berechnet die Division eines Vektors und eines Skalars
 * @param des der Ergebnisvektor
 * @param a Vektor
 * @param b Skalar
 */
void vecDivSca(CGVector3f *des, CGVector3f a, float b)
{
  (*des)[0] = a[0] / b;
  (*des)[1] = a[1] / b;
  (*des)[2] = a[2] / b;
}

/**
 * normalisiert den Vektor
 * @param des der Ergebnisvektor
 */
void vecNormalize(CGPoint3f *des)
{
  float length;
  vecLength(&length, (*des));
  vecDivSca(des, (*des), length);
}

/**
 * die Reflektion von einem Strahl 
*/
void vecReflect(CGPoint3f *des, CGPoint3f a, CGPoint3f norm)
{
  float length;
  CGPoint3f resNorm = {0.0, 0.0, 0.0};
  dot(&length, a, norm);
  vecMulSca(&resNorm, norm, length);
  vecSubVec(des, a, resNorm);
}

/*
 * gibt den maximalen wert zurueck
 */
float maxF(float a, float b)
{
  float result = 0.0;
  if (a >= b)
  {
    result = a;
  }
  else
  {
    result = b;
  }
  return result;  
}

/**
 * Berechnet eine Zufallszahl zwischen Maximum und Minimum
 * @param des das Ergebnis
 * @param max Maximum
 * @param min Minimum

void randomMaxMin(float *des, int max, int min)
{
  (*des) = (float)(rand() % (max + 1 - min)) + min;
  (*des) /= 10.0f;
} */

/* ---- Globale Funktionen ---- */

/**
 * initialisiert die beiden Structs mit den Vertices
 */
void initQuadStruct(void)
{
  // Back
  setVecV(&mainCube[0].A, L_G_B_Point);
  setVecV(&mainCube[0].B, L_C_B_Point);
  setVecV(&mainCube[0].C, R_G_B_Point);

  setVecV(&mainCube[1].A, L_C_B_Point);
  setVecV(&mainCube[1].B, R_C_B_Point);
  setVecV(&mainCube[1].C, R_G_B_Point);
  // Left
  setVecV(&mainCube[2].A, L_G_F_Point);
  setVecV(&mainCube[2].B, L_G_B_Point);
  setVecV(&mainCube[2].C, L_C_B_Point);

  setVecV(&mainCube[3].A, L_G_F_Point);
  setVecV(&mainCube[3].B, L_C_F_Point);
  setVecV(&mainCube[3].C, L_C_B_Point);
  // Front
  setVecV(&mainCube[4].A, L_G_F_Point);
  setVecV(&mainCube[4].B, L_C_F_Point);
  setVecV(&mainCube[4].C, R_G_F_Point);

  setVecV(&mainCube[5].A, L_C_F_Point);
  setVecV(&mainCube[5].B, R_C_F_Point);
  setVecV(&mainCube[5].C, R_G_F_Point);
  // Right
  setVecV(&mainCube[6].A, R_G_F_Point);
  setVecV(&mainCube[6].B, R_C_B_Point);
  setVecV(&mainCube[6].C, R_G_B_Point);

  setVecV(&mainCube[7].A, R_G_F_Point);
  setVecV(&mainCube[7].B, R_C_F_Point);
  setVecV(&mainCube[7].C, R_C_B_Point);
  // Ceiling
  setVecV(&mainCube[8].A, L_C_F_Point);
  setVecV(&mainCube[8].B, R_C_F_Point);
  setVecV(&mainCube[8].C, R_C_B_Point);

  setVecV(&mainCube[9].A, L_C_F_Point);
  setVecV(&mainCube[9].B, L_C_B_Point);
  setVecV(&mainCube[9].C, R_C_B_Point);
  // Ground
  setVecV(&mainCube[10].A, L_G_F_Point);
  setVecV(&mainCube[10].B, L_G_B_Point);
  setVecV(&mainCube[10].C, R_G_B_Point);

  setVecV(&mainCube[11].A, L_G_F_Point);
  setVecV(&mainCube[11].B, R_G_F_Point);
  setVecV(&mainCube[11].C, R_G_B_Point);

  // Red Cube
  // Back
  setVecV(&redCube[0].A, L_G_B_Quad);
  setVecV(&redCube[0].B, L_C_B_Quad);
  setVecV(&redCube[0].C, R_G_B_Quad);

  setVecV(&redCube[1].A, L_C_B_Quad);
  setVecV(&redCube[1].B, R_C_B_Quad);
  setVecV(&redCube[1].C, R_G_B_Quad);
  // Left
  setVecV(&redCube[2].A, L_G_F_Quad);
  setVecV(&redCube[2].B, L_G_B_Quad);
  setVecV(&redCube[2].C, L_C_B_Quad);

  setVecV(&redCube[3].A, L_G_F_Quad);
  setVecV(&redCube[3].B, L_C_F_Quad);
  setVecV(&redCube[3].C, L_C_B_Quad);
  // Front
  setVecV(&redCube[4].A, L_G_F_Quad);
  setVecV(&redCube[4].B, L_C_F_Quad);
  setVecV(&redCube[4].C, R_G_F_Quad);

  setVecV(&redCube[5].A, L_C_F_Quad);
  setVecV(&redCube[5].B, R_C_F_Quad);
  setVecV(&redCube[5].C, R_G_F_Quad);
  // Right
  setVecV(&redCube[6].A, R_G_F_Quad);
  setVecV(&redCube[6].B, R_C_B_Quad);
  setVecV(&redCube[6].C, R_G_B_Quad);

  setVecV(&redCube[7].A, R_G_F_Quad);
  setVecV(&redCube[7].B, R_C_F_Quad);
  setVecV(&redCube[7].C, R_C_B_Quad);
  // Ceiling
  setVecV(&redCube[8].A, L_C_F_Quad);
  setVecV(&redCube[8].B, R_C_F_Quad);
  setVecV(&redCube[8].C, R_C_B_Quad);

  setVecV(&redCube[9].A, L_C_F_Quad);
  setVecV(&redCube[9].B, L_C_B_Quad);
  setVecV(&redCube[9].C, R_C_B_Quad);
  // Ground
  setVecV(&redCube[10].A, L_G_F_Quad);
  setVecV(&redCube[10].B, L_G_B_Quad);
  setVecV(&redCube[10].C, R_G_B_Quad);

  setVecV(&redCube[11].A, L_G_F_Quad);
  setVecV(&redCube[11].B, R_G_F_Quad);
  setVecV(&redCube[11].C, R_G_B_Quad);
}

/**
 * Lichtberechnung nach Phong
 *
 * @param norm die Normalen
 */
void phong(CGPoint3f *result, CGPoint12f point)
{
  //float length = 0.0f;
  CGPoint3f ambient;
  CGPoint3f diffuse;
  CGPoint3f specular;
  CGPoint3f pos;
  CGPoint3f norm;
  float norm0;
  float norm1;
  CGPoint3f viewDir;
  CGPoint3f lightDir0;
  CGPoint3f lightDir1;
  CGPoint3f reflectDir0;
  CGPoint3f reflectDir1;
  CGPoint3f color;
  float spec = 0.0f;
  float spec0 = 0.0f;

  float diff = 0.0f;

  setVecf(&ambient, MATERIAL_DEFAULT_AMBIENT);
  setVecf(&diffuse, MATERIAL_DEFAULT_DIFFUSE);
  setVecf(&specular, MATERIAL_DEFAULT_SPECULAR);

  // diffuse
  vecSubVec(&lightDir0, lightPos0, pos);
  vecNormalize(&lightDir0);
  vecAbs(&lightDir0);

  vecSubVec(&lightDir1, lightPos1, pos);
  vecNormalize(&lightDir1);
  vecAbs(&lightDir1);

  dot(&norm0, norm, lightDir0);
  dot(&norm1, norm, lightDir1);

  diff = maxF(norm0, 0.0);
  diff += maxF(norm1, 0.0);

  vecMulSca(&color, color, diff);
  vecMulVec(&diffuse, diffuse, color);

  // specular
  vecSubVec(&viewDir, eyePos, pos);
  vecNormalize(&viewDir);

  vecMulSca(&lightDir0, lightDir0, -1.0f);
  vecMulSca(&lightDir1, lightDir1, -1.0f);
  
  vecReflect(&reflectDir0, lightDir0, norm);
  vecReflect(&reflectDir1, lightDir1, norm);

  dot(&spec, viewDir, reflectDir0);
  dot(&spec0, viewDir, reflectDir1);

  spec = maxF(spec, 0.0);
  spec += maxF(spec0, 0.0);

  spec = pow(spec, MATERIAL_DEFAULT_SHININESS);

  vecMulSca(&specular, color, spec);

  vecAddVec(result, (*result), ambient);
  vecAddVec(result, (*result), diffuse);
  vecAddVec(result, (*result), specular);
  vecAddSca(result, (*result), MATERIAL_DEFAULT_EMISSION);
}

/**
 * Schleife ueber alle Framebufferpixel
 */
void raycasting(void)
{
  CGPoint3f uPoint = {1.0f, 0.0f, 0.0f};
  CGPoint3f vPoint = {0.0f, 1.0f, 0.0f};
  CGPoint3f startPoint = {-1.0f, -1.0f, 0.0f};

  CGPoint3f delta = {0.0f, 0.0f, 0.0f};
  CGPoint3f deltaU = {0.0f, 0.0f, 0.0f};
  CGPoint3f deltaV = {0.0f, 0.0f, 0.0f};
  CGPoint3f posX = {0.0f, 0.0f, 0.0f};
  CGPoint3f posY = {0.0f, 0.0f, 0.0f};
  CGPoint3f projectionPoint = {0.0f, 0.0f, 0.0f};
  CGPoint3f direction = {0.0f, 0.0f, 0.0f};
  CGPoint3f intersectionPoint = {0.0f, 0.0f, 0.0f};

  CGPoint3f distance = {0.0f, 0.0f, 0.0f};

  CGPoint3f activeVec = {0.0f, 0.0f, 0.0f};
  CGPoint3f activeVec1 = {0.0f, 0.0f, 0.0f};
  CGPoint3f activeVec2 = {0.0f, 0.0f, 0.0f};
  CGPoint3f normVec = {0.0f, 0.0f, 0.0f};
  CGPoint3f normVec1 = {0.0f, 0.0f, 0.0f};
  CGPoint3f crossVec = {0.0f, 0.0f, 0.0f};

  CGPoint3f result = {0.0f, 0.0f, 0.0f};
  CGPoint3f resultWall = {0.0f, 0.0f, 0.0f};
  CGPoint3f resultCube = {0.0f, 0.0f, 0.0f};
  CGPoint3f nearestPoint = {0.0f, 0.0f, 0.0f};

  float alpha = NEAR;
  float length = 0.0f;
  float length1 = 0.0f;
  float length2 = 0.0f;
  float near = -1.0f;
  float x = 0.0f;

  deltaU[0] = uPoint[0] / FRAMEBUFFER_WIDTH * 2;

  deltaV[1] = vPoint[1] / FRAMEBUFFER_HEIGHT * 2;

  for (float j = 0; j < FRAMEBUFFER_HEIGHT; ++j)
  {
    for (float i = 0; i < FRAMEBUFFER_WIDTH; ++i)
    {
      // p = s + (i + 0,5) * du + (j + 0,5) * dv
      vecMulSca(&posX, deltaU, i + 0.5f);
      vecMulSca(&posY, deltaV, j + 0.5f);
      vecAddVec(&delta, posX, posY);
      vecAddVec(&projectionPoint, delta, startPoint);
      //fprintf(stdout, "projectionPoint[0] %6f, projectionPoint[1] %f \n",projectionPoint[0], projectionPoint[1]);

      // u = (p − e)
      vecSubVec(&intersectionPoint, projectionPoint, eyePos);
      vecLength(&length, intersectionPoint);
      vecNormalize(&intersectionPoint);
      vecDivSca(&direction, intersectionPoint, length);
      // fprintf(stdout, "direction: %f, direction: %f, direction: %f\n", direction[0], direction[1], direction[2]);

      // Kugel
      {
        // alpha = -(e - c) * u
        vecSubVec(&distance, eyePos, blueBallPosition);
        dot(&alpha, distance, direction);
        alpha *= -1;

        vecMulSca(&intersectionPoint, direction, alpha);

        vecAddVec(&intersectionPoint, intersectionPoint, eyePos);

        // fprintf(stdout, "intersectionPoint: %f, intersectionPoint: %f, intersectionPoint: %f\n", intersectionPoint[0],intersectionPoint[1],intersectionPoint[2]);
        vecSubVec(&distance, intersectionPoint, blueBallPosition);
        vecLength(&length, distance);

        // fprintf(stdout, "distance: %f, distance: %f, distance: %f\n", distance[0],distance[1],distance[2]);
        if (length <= RADIUS)
        {
          x = sqrt(pow(RADIUS, 2) - pow(length, 2));
          if (alpha >= x)
          {
            // Opake Kugel
            // result = p + (alpha - x) * u;
            vecMulSca(&result, direction, alpha - x);
            vecAddVec(&result, result, eyePos);
            vecSubVec(&nearestPoint, result, eyePos);
            vecLength(&near, nearestPoint);
          }
          else if (alpha + x > 0)
          {
            // Transparente Kugel
            // result = p + (alpha + x) * u;
            vecMulSca(&result, direction, alpha + x);
            vecAddVec(&result, result, eyePos);
            vecSubVec(&nearestPoint, result, eyePos);
            vecLength(&near, nearestPoint);
          }
        }
      }
      // Auszenwaende
      {
        for (int i = 0; i < 12; i++)
        {
          setVecV(&activeVec, mainCube[i].A);
          setVecV(&activeVec1, mainCube[i].B);
          setVecV(&activeVec2, mainCube[i].C);

          vecSubVec(&normVec, activeVec1, activeVec);
          vecSubVec(&normVec1, activeVec2, activeVec);

          cross(&crossVec, normVec, normVec1);
          vecLength(&length, normVec);
          vecDivSca(&normVec, normVec, length);

          dot(&length, normVec, activeVec);
          dot(&length1, eyePos, normVec);
          dot(&length2, direction, normVec);

          alpha = (length - length1) / length2;

          if (alpha > 0.0f && length2 != 0.0f)
          {
            vecMulSca(&resultWall, direction, alpha);
            vecAddVec(&resultWall, resultWall, eyePos);
            vecSubVec(&nearestPoint, resultWall, eyePos);
            vecLength(&length, nearestPoint);
            if (length < near)
            {
              near = length;
              setVecV(&result, resultWall);
            }
          }
        }
      }
      // redCube
      {
        
      }
      // 𝐼 Ԧ𝑝 = 𝐼𝑙𝑜𝑘𝑎𝑙 Ԧ𝑝, Ԧ𝑒 + 𝑘𝑟𝑒𝑓𝑙 ∙ 𝐼 Ԧ𝑠𝑟𝑒𝑓𝑙 + 𝑘𝑔𝑒𝑏𝑟 ∙ 𝐼 Ԧ𝑠𝑡𝑟𝑎𝑛𝑠
      // Intensitaet am result berechnen
      // I(p) = Ilokal(p,e) + krefl * I(srefl) + kgebr * I(Strans)
    }
  }
}

/**
 * Zeichen-Funktion fuer den Hilfe-Screen.
 */
void drawHelp(void)
{
  fprintf(stdout, "Tastenbelegung\n");
  fprintf(stdout, "q/Q/Esc : Beendet das Programm.\n");
  fprintf(stdout, "h/H : Ruft diese Hilfeausgabe auf und pausiert das Spiel.\n");
  fprintf(stdout, "F1/F2 : Lichtdarstellung\n");
}

/*
 * Gibt die Position der blauen Kugel zurück.
 */
CGPoint3f *getBlueBallPos(void)
{
  return &blueBallPosition;
}

/*
 * Gibt die Position der blauen Kugel zurück.
 */
CGPoint3f *getRedCubePos(void)
{
  return &redCubePos;
}

/*
 * Gibt die Position der blauen Kugel zurück.
 */
CGPoint3f *getGreenModelPos(void)
{
  return &greenModelPosition;
}

/**
 * gibt die Lichtposition zurueck
 */
CGPoint4f *getLightPos(int idx)
{
  CGPoint4f *lightPos;
  if (idx == 0)
  {
    lightPos = &(lightPos0);
  }
  else
  {
    lightPos = &(lightPos1);
  }

  return lightPos;
}

/**
 * schaltet die Lichter an und ab
 */
void toggleLight(int idx)
{
  switch (idx)
  {
  case 0:
    activeLight0 = !activeLight0;
    break;
  case 1:
    activeLight1 = !activeLight1;
    break;
  }
}

/**
 * schaltet die Lichter an und ab
 */
int getLightActive(int idx)
{
  int active;
  switch (idx)
  {
  case 0:
    active = activeLight0;
    break;
  case 1:
    active = activeLight1;
    break;
  }
  return active;
}

void toggleDrawBlue(void)
{
  drawBlue = !drawBlue;
}

int getDrawBlue(void)
{
  return drawBlue;
}

void toggleDrawRed(void)
{
  drawRed = !drawRed;
}

int getDrawRed(void)
{
  return drawRed;
}