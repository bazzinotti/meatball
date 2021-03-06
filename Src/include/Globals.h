#ifndef __GLOBALS_H__
#define __GLOBALS_H__

// You can define DEMO mode to do specific things for when demoing to your friends :) 
//#define DEMO

//#include <stdlib.h>
//#include <stdio.h>
//#include <time.h>
//#include <fstream>

#include "OS.h"
// Global Big Definitions (important)
#include "defines.h"
//#include "Misc.h"
#include <sstream>
extern std::stringstream error;
void PrintErrorToConsole();
void ClearErrorStream();

// The next lines make DEBUGLOG function a printf if we are a _DEBUG build, but make it do nothing for a Release Build.
#ifdef _DEBUG
// Update the following if needed…
#define DEBUGLOG(...) printf(__VA_ARGS__)
#else
#define DEBUGLOG(...) do {} while (0)
#endif


// Set the Transparent Color for Bitmaps / Pngs
void SetColorKey(Uint32);



/// Draws the Framerate
void FramerateDraw( SDL_Renderer *renderer, double posx = 5.0, double posy = 5.0 );




extern cMouseCursor *pMouse;		// The Mouse
	
/// @addtogroup Engine_Meatball
/// These are classes Meatball uses from the SDL_ep Engine Library.
/// SDL_ep is the Home-grown engine that Meatball is built on. IT's still
/// in development. Since it kind of develops alongside of meatball. It stands for Engine Primitives
// @{
extern cFramerate *pFramerate;
extern cImageManager *IMan;
extern cSoundManager *SMan;
extern cFont *pFont;
extern cAudio *pAudio;
extern cTCPNet *pTCP;
// @}

/// finished with game or not?
//extern int done;

/// game paused?
extern SDL_bool paused;

// Fonts
extern TTF_Font *Menu_Font;
extern TTF_Font *bold_16;

/// the Screen
extern Window window;

/// Keyboard Keystate
extern const Uint8 *keys; // migration

/// events
extern SDL_Event event;

/// fps display SDL_boolean
//extern SDL_bool fps_display;

/// Magenta (default) Colorkey
extern Uint32 colorkey;

/// The Current mode
//extern Uint8 mode, oldmode;

extern SDL_Renderer *Renderer;


#endif