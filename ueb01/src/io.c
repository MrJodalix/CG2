/**
 * @file
 * Ein-/Ausgabe-Modul.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
 *
 * @author Joshua-Scott Schoettke, Ilana Schmara (Gruppe 1).
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "types.h"
#include "logic.h"
#include "scene.h"
#include "texture.h"
#include "debugGL.h"

/* ---- Konstanten ---- */
/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60

/**
 * Umschalten zw. Vollbild- und Fenstermodus.
 * Beim Wechsel zum Fenstermodus wird vorherige Position und Groesse
 * wiederhergestellt. HINWEIS: Fenster wird nicht korrekt auf alte Position
 * gesetzt, da GLUT_WINDOW_WIDTH/HEIGHT verfaelschte Werte liefert.
 */
static void
toggleFullscreen(void)
{
  /* Flag: Fullscreen: ja/nein */
  static GLboolean fullscreen = GL_FALSE;
  /* Zwischenspeicher: Fensterposition */
  static CGPoint2i windowPos;
  /* Zwischenspeicher: Fenstergroesse */
  static CGDimensions2i windowSize;

  /* Modus wechseln */
  fullscreen = !fullscreen;

  if (fullscreen)
  {
    /* Fenstereinstellungen speichern */
    windowPos[0] = glutGet(GLUT_WINDOW_X);
    windowPos[1] = glutGet(GLUT_WINDOW_Y);
    windowSize[0] = glutGet(GLUT_WINDOW_WIDTH);
    windowSize[1] = glutGet(GLUT_WINDOW_HEIGHT);
    /* In den Fullscreen-Modus wechseln */
    glutFullScreen();
  }
  else
  {
    /* alte Fenstereinstellungen wiederherstellen */
    glutReshapeWindow(windowSize[0], windowSize[1]);

    /* HINWEIS:
       Auskommentiert, da es sonst Probleme mit der Vollbildarstellung bei
       Verwendung von FreeGlut gibt */
    glutPositionWindow(windowPos[0], windowPos[1]);
  }
}

/**
 * Setzen der Projektionsmatrix.
 * Setzt die Projektionsmatrix unter Berücksichtigung des Seitenverhältnisses
 * des Anzeigefensters, sodass das Seitenverhältnisse der Szene unverändert
 * bleibt und gleichzeitig entweder in x- oder y-Richtung der Bereich von -1
 * bis +1 zu sehen ist.
 * @param aspect Seitenverhaeltnis des Anzeigefensters (In).
 */
static void
setProjection(GLdouble aspect)
{
  /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  /* Koordinatensystem bleibt quadratisch */
  if (aspect <= 1)
  {
    gluOrtho2D(-1.0, 1.0,                    /* links, rechts */
               -1.0 / aspect, 1.0 / aspect); /* unten, oben */
  }
  else
  {
    gluOrtho2D(-1.0 * aspect, 1.0 * aspect, /* links, rechts */
               -1.0, 1.0);                  /* unten, oben */
  }
}

/**
 * Debug-Ausgabe eines Mauseignisses.
 * Ausgabe des Ereignisses und der Position des Mauszeigers.
 * Die Ausgabe erfolgt nur, wenn das Makro DEBUG definiert ist.
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param eventType Typ des Ereignisses.
 * @param button ausloesende Maustaste (nur bei Ereignissen vom Typ mouseButton).
 * @param buttonState Status der Maustaste (nur bei Ereignissen vom Typ mouseButton).
 */
static void
debugMouseEvent(int x, int y, CGMouseEventType eventType, int button,
                int buttonState)
{
#ifdef DEBUG
  switch (eventType)
  {
  case mouseButton:
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
      printf("Linke");
      break;
    case GLUT_MIDDLE_BUTTON:
      printf("Mittlere");
      break;
    case GLUT_RIGHT_BUTTON:
      printf("Rechte");
      break;
    }

    printf(" Maustaste");

    /* Status ausgeben */
    if (buttonState == GLUT_DOWN)
    {
      printf(" gedrueckt.\n");
    }
    else
    {
      printf(" losgelassen.\n");
    }
    break;
  case mouseMotion:
    printf("Mausbewegung bei gedrueckter Maustaste\n");
    break;
  case mousePassiveMotion:
    printf("Mausbewegung bei nicht gedrueckter Maustaste\n");
    break;
  }

  printf("  Position der Maus: (%d,%d)\n\n", x, y);
#else
  (void)x;
  (void)y;
  (void)eventType;
  (void)buttonState;
  (void)button;
#endif
}

/**
 * Debug-Ausgabe eines Tasturereignisses.
 * Ausgabe der Taste, die das Ereignis ausgeloest hat, des Status der Modifier
 * und der Position des Mauszeigers.
 * Die Ausgabe erfolgt nur, wenn das Makro DEBUG definiert ist.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 */
static void
debugPrintKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x,
                        int y)
{
  (void)isSpecialKey;
  (void)key;
  (void)status;
  (void)x;
  (void)y;
#ifdef DEBUG
  /* Bitmask mit Status der Modifier-Tasten */
  int modifierMask = glutGetModifiers();

  /* Spezialtaste */
  if (isSpecialKey)
  {
    printf("Spezialtaste");

    switch (key)
    {
    case GLUT_KEY_F1:
      printf(" F1");
      break;
    case GLUT_KEY_F2:
      printf(" F2");
      break;
    case GLUT_KEY_UP:
      printf(" UP");
      break;
    case GLUT_KEY_DOWN:
      printf(" DOWN");
      break;
    case GLUT_KEY_LEFT:
      printf(" LEFT");
      break;
    case GLUT_KEY_RIGHT:
      printf(" RIGHT");
      break;
    case GLUT_KEY_HOME:
      printf(" HOME");
      break;
    case GLUT_KEY_END:
      printf(" END");
      break;
    case GLUT_KEY_INSERT:
      printf(" INSERT");
      break;
    }
  }
  /* keine Spezialtaste */
  else
  {
    printf("Taste %c (ASCII-Code: %u)", key, key);
  }

  /* Status ausgeben */
  if (status == GLUT_DOWN)
  {
    printf(" gedrueckt.\n");
  }
  else
  {
    printf(" losgelassen.\n");
  }

  /* Status der Modifier ausgeben */
  printf("  Status der Shift-Taste: %d, Status der Strg-Taste: %d, Status der Alt-Taste: %d\n",
         modifierMask & GLUT_ACTIVE_SHIFT, (modifierMask & GLUT_ACTIVE_CTRL) >> 1,
         (modifierMask & GLUT_ACTIVE_ALT) >> 2);

  printf("  Position der Maus: (%d,%d)\n\n", x, y);

#endif
}

/**
 * Verarbeitung eines Mausereignisses.
 * Durch Bewegung der Maus bei gedrueckter Maustaste kann die aktuelle
 * Zeichenfarbe beeinflusst werden.
 * Falls Debugging aktiviert ist, wird jedes Mausereignis auf stdout
 * ausgegeben.
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param eventType Typ des Ereignisses.
 * @param button ausloesende Maustaste (nur bei Ereignissen vom Typ mouseButton).
 * @param buttonState Status der Maustaste (nur bei Ereignissen vom Typ mouseButton).
 */
static void
handleMouseEvent(int x, int y, CGMouseEventType eventType, int button,
                 int buttonState)
{
  /* aktueller Status der drei Maustasten */
  static int mouseButtonStates[3] = {GLUT_UP, GLUT_UP, GLUT_UP};

  /* x-Position der Maus beim letzten Setzen Farbe ueber die jeweilige
     Maustaste */
  static int oldMousePos[3] = {0, 0, 0};

  debugMouseEvent(x, y, eventType, button, buttonState);

  switch (eventType)
  {
  case mouseButton:
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
      mouseButtonStates[0] = buttonState;
      oldMousePos[0] = x;
      oldMousePos[1] = y;
      break;
    }
    break;
  case mouseMotion:
    if (mouseButtonStates[0] == GLUT_DOWN)
    {
      oldMousePos[0] = x;
      oldMousePos[1] = y;
    }
    /* Farbwert im Logik-Modul aktualisieren */
    calcMousePosition(oldMousePos[0], oldMousePos[1], (GLfloat)glutGet(GLUT_WINDOW_WIDTH), (GLfloat)glutGet(GLUT_WINDOW_HEIGHT));

    /* Neuzeichnen anstossen */
    glutPostRedisplay();
    break;
  default:
    break;
  }
}

/**
 * Mouse-Button-Callback.
 * @param button Taste, die den Callback ausgeloest hat.
 * @param state Status der Taste, die den Callback ausgeloest hat.
 * @param x X-Position des Mauszeigers beim Ausloesen des Callbacks.
 * @param y Y-Position des Mauszeigers beim Ausloesen des Callbacks.
 */
static void
cbMouseButton(int button, int state, int x, int y)
{
  handleMouseEvent(x, y, mouseButton, button, state);
}

/**
 * Mouse-Motion-Callback.
 * @param x X-Position des Mauszeigers.
 * @param y Y-Position des Mauszeigers.
 */
static void
cbMouseMotion(int x, int y)
{
  handleMouseEvent(x, y, mouseMotion, 0, 0);
}

/**
 * Mouse-Passive-Motion-Callback.
 * @param x X-Position des Mauszeigers.
 * @param y Y-Position des Mauszeigers.
 */
static void
cbMousePassiveMotion(int x, int y)
{
  handleMouseEvent(x, y, mousePassiveMotion, 0, 0);
}

/**
 * Verarbeitung eines Tasturereignisses.
 * Pfeiltasten steuern die Position des angezeigten Rechtecks.
 * F1-Taste (de-)aktiviert Wireframemodus.
 * F2-Taste schaltet zwischen Fenster und Vollbilddarstellung um.
 * ESC-Taste und q, Q beenden das Programm.
 * Falls Debugging aktiviert ist, wird jedes Tastaturereignis auf stdout
 * ausgegeben.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 */
static void
handleKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x,
                    int y)
{
  /** Keycode der ESC-Taste */
#define ESC 27

  /* temporaere Variable fuer Zustaende */
  int state = 0;

  /* Debugausgabe */
  debugPrintKeyboardEvent(key, status, isSpecialKey, x, y);

  /* Taste gedrueckt */
  if (status == GLUT_DOWN)
  {
    /* Spezialtaste gedrueckt */
    if (isSpecialKey)
    {
      switch (key)
      {
        /* (De-)Aktivieren des Wireframemode */
      case GLUT_KEY_F1:
        toggleWireframeMode();
        glutPostRedisplay();
        break;
        /* Umschalten zwischen Fenster- und Vollbilddarstellung */
      case GLUT_KEY_F2:
        toggleFullscreen();
        break;
      case GLUT_KEY_F3:
        state = !getTexturingStatus();
        setTexturingStatus(state);
        glutPostRedisplay();
        break;
      }
    }
    /* normale Taste gedrueckt */
    else
    {
      switch (key)
      {
      case '+':
        addVertices();
        break;
      case '-':
        deleteVertices();
        break;
      case 'b':
      case 'B':
        if (*getCurrentLevel() == 1)
        {
          toggleBezier();
        }
        break;
      case 'c':
      case 'C':
        toggleHull();
        break;
      case 'h':
      case 'H':
        toggleHelp();
        break;
      case 'k':
      case 'K':
        togglePause();
        break;
      case 'p':
      case 'P':
        togglePause();
        break;
      case 'n':
      case 'N':
        toggleNormals();
        break;
      case 'q':
      case 'Q':
      case ESC:
        deleteDisplayLists();
        freeAll();
        exit(0);
        break;
      }
    }
  }
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbKeyboard(unsigned char key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_DOWN, GL_FALSE, x, y);
}

/**
 * Callback fuer Tastenloslassen.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Loslassens (In).
 * @param y y-Position der Maus zur Zeit des Loslassens (In).
 */
static void
cbKeyboardUp(unsigned char key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_UP, GL_FALSE, x, y);
}

/**
 * Callback fuer Druck auf Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbSpecial(int key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_DOWN, GL_TRUE, x, y);
}

/**
 * Callback fuer Loslassen von Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Loslassens (In).
 * @param y y-Position der Maus zur Zeit des Loslassens (In).
 */
static void
cbSpecialUp(int key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_UP, GL_TRUE, x, y);
}

/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und anschliessendes Neuzeichnen,
 * setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer(int lastCallTime)
{
  int *pause = getPause();
  /* Seit dem Programmstart vergangene Zeit in Millisekunden */
  int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

  /* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden oder
    0 wenn die Pause aktiviert ist*/
  double interval;
  if (*pause != 1)
  {
    interval = (double)(thisCallTime - lastCallTime) / 1000;
  }

  if (thisCallTime % 500 < interval * 1000)
  {
    calcRocketPosition(interval);
  }

  /* Wieder als Timer-Funktion registrieren */
  glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

  /* Neuzeichnen anstossen */
  glutPostRedisplay();
}

/**
 * Callback fuer Aenderungen der Fenstergroesse.
 * Initiiert Anpassung der Projektionsmatrix an veränderte Fenstergroesse.
 * @param w Fensterbreite (In).
 * @param h Fensterhoehe (In).
 */
static void
cbReshape(int w, int h)
{
  /* Das ganze Fenster ist GL-Anzeigebereich */
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  /* Anpassen der Projektionsmatrix an das Seitenverhältnis des Fensters */
  setProjection((GLdouble)w / (GLdouble)h);
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay(void)
{
  /* Buffer zuruecksetzen */
  glClear(GL_COLOR_BUFFER_BIT);

  /* Nachfolgende Operationen beeinflussen Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  /* Szene zeichnen */
  drawScene();

  /* Objekt anzeigen */
  glutSwapBuffers();
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void
registerCallbacks(void)
{
  /* Mouse-Button-Callback (wird ausgefuehrt, wenn eine Maustaste
   * gedrueckt oder losgelassen wird) */
  glutMouseFunc(cbMouseButton);

  /* Mouse-Motion-Callback (wird ausgefuehrt, wenn die Maus bewegt wird,
   * waehrend eine Maustaste gedrueckt wird) */
  glutMotionFunc(cbMouseMotion);

  /* Mouse-Motion-Callback (wird ausgefuehrt, wenn die Maus bewegt wird,
   * waehrend keine Maustaste gedrueckt wird) */
  glutPassiveMotionFunc(cbMousePassiveMotion);

  /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
  glutKeyboardFunc(cbKeyboard);

  /* Tasten-Loslass-Callback - wird ausgefuehrt, wenn eine Taste losgelassen
   * wird */
  glutKeyboardUpFunc(cbKeyboardUp);

  /* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
   * (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
   * Einfuegen) gedrueckt wird */
  glutSpecialFunc(cbSpecial);

  /* Spezialtasten-Loslass-Callback - wird ausgefuehrt, wenn eine Spezialtaste
   * losgelassen wird */
  glutSpecialUpFunc(cbSpecialUp);

  /* Automat. Tastendruckwiederholung ignorieren */
  glutIgnoreKeyRepeat(1);

  /* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
  glutTimerFunc(1000 / TIMER_CALLS_PS,       /* msecs - bis Aufruf von func */
                cbTimer,                     /* func  - wird aufgerufen    */
                glutGet(GLUT_ELAPSED_TIME)); /* value - Parameter, mit dem
                                                func aufgerufen wird */

  /* Reshape-Callback - wird ausgefuehrt, wenn neu gezeichnet wird (z.B. nach
   * Erzeugen oder Groessenaenderungen des Fensters) */
  glutReshapeFunc(cbReshape);

  /* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
   * Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
  glutDisplayFunc(cbDisplay);
}

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO(char *title, int width, int height)
{
  int windowID = 0;

  /* Kommandozeile immitieren */
  int argc = 1;
  char *argv = "cmd";

  /* Glut initialisieren */
  glutInit(&argc, &argv);

  /* DEBUG-Ausgabe */
  INFO(("Erzeuge Fenster...\n"));

  /* Initialisieren des Fensters */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);

  /* Fenster erzeugen */
  windowID = glutCreateWindow(title);

  if (windowID)
  {

    /* DEBUG-Ausgabe */
    INFO(("...fertig.\n\n"));

    /* DEBUG-Ausgabe */
    INFO(("Initialisiere Szene...\n"));

    if (initScene())
    {
      /* DEBUG-Ausgabe */
      INFO(("...fertig.\n\n"));

      /* DEBUG-Ausgabe */
      INFO(("Lade und initialisiere Texturen...\n"));

      if (initTextures())
      {
        /* DEBUG-Ausgabe */
        INFO(("...fertig.\n\n"));

        /* DEBUG-Ausgabe */
        INFO(("Registriere Callbacks...\n"));

        registerCallbacks();

        /* DEBUG-Ausgabe */
        INFO(("...fertig.\n\n"));

        /* DEBUG-Ausgabe */
        INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

        glutMainLoop();
      }
      else
      {
        /* DEBUG-Ausgabe */
        INFO(("...fehlgeschlagen.\n\n"));

        glutDestroyWindow(windowID);
        windowID = 0;
      }
    }
    else
    {
      /* DEBUG-Ausgabe */
      INFO(("...fehlgeschlagen.\n\n"));

      glutDestroyWindow(windowID);
      windowID = 0;
    }
  }
  else
  {
    /* DEBUG-Ausgabe */
    INFO(("...fehlgeschlagen.\n\n"));
  }

  return windowID;
}
