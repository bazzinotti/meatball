#include "MasterBlaster.h"
#include "Camera.h"
#include "console.h"
#include "level.h"
#include "player.h"

MasterBlaster *MB;

MasterBlaster::MasterBlaster()
{
	fps_display = SDL_FALSE;
	mode = MODE_MAINMENU;
	
#ifdef INGAME_LEVEL_EDITOR
	pLevelEditor = new cLevelEditor();
#endif
	pConsole = new cConsole();
	
	pGame = new cGame();
	pMainMenu = new cMainMenu();
	
	// init Game
	pGame->Init();
}

MasterBlaster::~MasterBlaster()
{
				
#ifdef INGAME_LEVEL_EDITOR
	if( pLevelEditor )
	{
		delete pLevelEditor;
	}
#endif
	if ( pConsole )
	{
		delete pConsole;
	}
	if (pGame) {
		delete pGame;
	}
	if (pMainMenu)
	{
		delete pMainMenu;
	}

}

int MasterBlaster::Game()
{
    pAudio->SetMusicVolume(127);
    pPlayer->beatlevel = SDL_FALSE;
    pPlayer->visible = SDL_TRUE;
    pPlayer->Health = 70;
	// Only load the level on the first time we press START GAME
	if (oldmode == MODE_MAINMENU)
	{
		string levelname = "test.lvl";
		DEBUGLOG ("loading level: %s\n", levelname.c_str());
		pLevel->Load( levelname );	// load the level
		DEBUGLOG ("Finished loading level\n");
        //string filename = DIR_MUSIC + pLevel->Musicfile;
        //pAudio->PlayMusik((char*)filename.c_str(), 1);
	}
    pCamera->SetPos( pPlayer->posx - pCamera->x - window.w, 0 );
    
	return pGame->Do();
}

int MasterBlaster::MainMenu()
{
    pAudio->SetMusicVolume(127);	
	pAudio->PlayMusik( MAINMENU_MUSIC );
	
	int mode = pMainMenu->Do();
	
	return mode;
}

int MasterBlaster::Console()
{
	int rval = pConsole->Do();
	if (rval == MODE_QUIT)
		return MODE_QUIT;
	else
		return oldmode;
}

int MasterBlaster::LevelEditor()
{
	return pLevelEditor->Do();
}

void MasterBlaster::Loop()
{
	register Uint8 newmode = MODE_MAINMENU; // for first time
	while( mode != MODE_QUIT )
	{
		oldmode = mode;
		mode = newmode;
		
		if (mode == MODE_MAINMENU)
			newmode = MainMenu();
		
		else if (mode == MODE_GAME)
		{
			newmode = Game();
		}
		#ifdef INGAME_LEVEL_EDITOR
			else if (mode == MODE_LEVELEDITOR)
			{
				newmode = LevelEditor();
			}
		#endif
		else if (mode == MODE_CONSOLE)
			newmode = Console();
	}
}