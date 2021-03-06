#include "Globals.h"
#include "MiniEngine.h"
#include "MainMenu.h"
#include "Game.h"
#include "LevelEditor.h"
#include "Console.h"



class MasterBlaster
{
public:
	MasterBlaster();
	~MasterBlaster();
	
	
	void Loop();
	int Game();
	int MainMenu();
	int Console();
	int LevelEditor();
	
	Uint8 mode, oldmode;
	cLevelEditor *pLevelEditor;
	
	
	cGame *pGame;
	cMainMenu *pMainMenu;
	
	SDL_bool fps_display;
};

extern MasterBlaster *MB;