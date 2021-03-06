
#include "level.h"
#include "effects.h"
#include "bullet.h"
#include "enemy.h"
#include <fstream>
#include "player.h"

// Used to set player's start position
extern cPlayer *pPlayer;

using namespace std;

// Global!
cLevel *pLevel;


cLevelData :: cLevelData( void ) :
BasicSprites(OM_OBLITERATE_OBJS_AT_DESTROY, OM_DELETE_OBJS)
{
	BG_red = 255;
	BG_green = 255;
	BG_blue = 255;
}

cLevelData :: ~cLevelData( void )
{	
	BasicSprites.~ObjectManager();
}

void cLevelData :: AddSprite( cMVelSprite *Sprite )
{
	BasicSprites.add(Sprite);
}

int cLevelData :: GetCollidingSpriteNum( SDL_Rect *Crect )
{
	if( BasicSprites.objects && Crect )
	{
		for( register unsigned int i = 0; i < BasicSprites.objcount; i++ )
		{
			if( !BasicSprites.objects[i] ) 
			{
				continue;
			}
            
			if( RectIntersect( &(const SDL_Rect&)BasicSprites.objects[i]->GetRect( SDL_TRUE ), Crect ) )
			{
				return i;
			}
		}
	}

	return -1;
}

// This function takes external pointers
SDL_bool cLevelData :: GetAllCollidingSpriteNum( SDL_Rect *Crect, ObjectManager<cMVelSprite> &obj_man )
{
	SDL_bool did_we_get_objects = SDL_FALSE;
	
	if( BasicSprites.objects && Crect )
	{
		//DEBUGLOG( "Using Crect coords x: %d, y: %d, width: %d, height: %d\n", Crect->x,Crect->y, Crect->w, Crect->h);
		
		for( register unsigned int i = 0; i < BasicSprites.objcount; i++ )
		{
			if( !BasicSprites.objects[i] )
			{
				continue;
			}
            
			if( RectIntersect( &(const SDL_Rect&)BasicSprites.objects[i]->GetRect( SDL_TRUE ), Crect ) )
			{
				// Add this sprite num to the list
				if (obj_man.hasa(BasicSprites.objects[i]) < 0)
					obj_man.add(BasicSprites.objects[i]);
				
				did_we_get_objects = SDL_TRUE;
			}
		}
	}
	
	return did_we_get_objects;
}

cBasicSprite *cLevelData :: GetCollidingSprite( SDL_Rect *Crect )
{
	if( BasicSprites.objects && Crect )
	{
		for( register unsigned int i = 0; i < BasicSprites.objcount; i++ )
		{
			if( !BasicSprites.objects[i] ) 
			{
				continue;
			}

			if( RectIntersect( &(const SDL_Rect&)BasicSprites.objects[i]->GetRect( SDL_FALSE ), Crect ) )
			{
				return BasicSprites.objects[i];
			}
		}
	}

	return NULL;
}

void cLevelData :: DeleteSprite( unsigned int number )
{
	// WHY???
	//if( (BasicSprites && number) < (EnemyCount && BasicSprites[number]) ) 
	//{
		cout<<"deleted "<<number;
		delete BasicSprites.objects[number];
		BasicSprites.objects[number] = NULL;
	//}
}

cLevel :: cLevel( void )
{
	levelfile = "none";
	pLevelData_Layer1 = NULL;
}

cLevel :: ~cLevel( void )
{
	Unload();
}

void cLevel :: Load( string filename )
{
	string full_filename = DIR_LEVEL + filename;

	if( !FileValid( full_filename ) )
	{
		DEBUGLOG( "Error : Level does not exist : %s\n", full_filename.c_str() );
		return;
	}

	Unload();
	
	DeleteAllParticleEmitter();
	DeleteAllBullets();
	DeleteAllEnemies();
	
	levelfile = filename;

	pLevelData_Layer1 = new cLevelData();

	ifstream ifs;
	ifs.open( full_filename.c_str(), ios :: in );

	char *contents = new char[500]; // maximal length of an line
	
	for( register unsigned int i = 0; ifs.getline( contents, 500 ); i++ )
	{
		Parse( contents, i );
	}

	ifs.close();

	delete contents;
	
	pFramerate->Reset();
    //string musicpath = DIR_MUSIC + Musicfile;
    //pAudio->PlayMusik( (char *)musicpath.c_str(),0,SDL_TRUE );
}

void cLevel :: Unload( void )
{
	if( pLevelData_Layer1 ) 
	{
		delete pLevelData_Layer1;
		pLevelData_Layer1 = NULL;
	}

	levelfile.clear();
}

void cLevel :: Save( void )
{
	string filename = DIR_LEVEL;
	filename += levelfile.c_str();

	if( !FileValid( filename ) ) 
	{
		DEBUGLOG( "Error Level Save : Level file does not exist %s\n", filename.c_str() );
	}

	ofstream out( filename.c_str(), ios::out );
	ofs = &out;

	if( !ofs )
	{
		DEBUGLOG( "Error Level Save: could not open the Level file %s\n", filename.c_str() );
		return;
	}
	
	PrintSaveHeader();
	SavePlayerPos();
	SaveMusicFile();
	SaveMapObjects();
	SaveMapEnemies();

	DEBUGLOG( "Level saved to %s\n", levelfile.c_str() );
	
	ofs = NULL;
}

void cLevel :: SaveToFile( string &filename )
{
	string absfilename = DIR_LEVEL;
	absfilename += filename;
	
	/*if( !FileValid( filename ) )
	{
		DEBUGLOG( "Error Level Save : Level file does not exist %s\n", filename.c_str() );
	}*/
	
	ofstream out( absfilename.c_str(), ios::out );
	ofs = &out;
	
	if( !ofs )
	{
		DEBUGLOG( "Error Level Save: could not open the Level file %s\n", filename.c_str() );
		return;
	}
	
	PrintSaveHeader();
	SavePlayerPos();
	SaveMusicFile();
	SaveMapObjects();
	SaveMapEnemies();
	
	DEBUGLOG( "Level saved to %s\n", filename.c_str() );
	
	ofs = NULL;
}

void cLevel :: PrintSaveHeader()
{
	sprintf( row,  "### Level Saved with MeatBall FX V.0 ###\n\n" );
	ofs->write( row, strlen( row ) );
	
	sprintf( row,  "### Level Saved on %s\n", Get_Curr_Date().c_str() );
	ofs->write( row, strlen( row ) );
}

void cLevel :: SavePlayerPos()
{
	sprintf( row, "Player %d %d\n", (int) pPlayer->Startposx, /*(int)window.h -*/ (int)pPlayer->Startposy );
	ofs->write( row, strlen( row ) );
}

void cLevel :: SaveMusicFile()
{
	sprintf( row,  "Music %s\n\n", Musicfile.c_str() );
	ofs->write( row, strlen( row ) );
}

void cLevel :: SaveMapObjects()
{
	sprintf( row, "### Map Objects ###\n" );
	ofs->write( row, strlen( row ) );
	
	// Map Objects
	if( pLevelData_Layer1 )
	{
		for( register unsigned int i = 0; pLevelData_Layer1->BasicSprites.objcount > i; i++ )
		{
			if( !pLevelData_Layer1->BasicSprites.objects[i] )
			{
				continue;
			}
			
			string image_filename = IMan->GetIdentifier( pLevelData_Layer1->BasicSprites.objects[i]->srcimage );
			if (image_filename.empty())
			{
				cout<<"Um... WTF  I don't get it...."<<endl;
				cout << "i=" << i << endl << "object[i] = "<< pLevelData_Layer1->BasicSprites.objects[i] << endl;
				cout << "We won't save the level then.. or let;s just skip this sprite.. how about that :P"<<endl;
				//return;
				continue;
			}
			
			image_filename.erase( 0, strlen( DIR_PIXMAPS ) );
			
			if( pLevelData_Layer1->BasicSprites.objects[i]->type == SPRITE_TYPE_SOLID )
			{
				sprintf(row,  "Sprite %s %d %d SOLID\n", image_filename.c_str(), (int) pLevelData_Layer1->BasicSprites.objects[i]->posx, (int)window.h - (int)pLevelData_Layer1->BasicSprites.objects[i]->posy );
			}
			else if( pLevelData_Layer1->BasicSprites.objects[i]->type == SPRITE_TYPE_PASSIVE )
			{
				sprintf(row,  "Sprite %s %d %d PASSIVE\n", image_filename.c_str(), (int) pLevelData_Layer1->BasicSprites.objects[i]->posx, (int)window.h - (int)pLevelData_Layer1->BasicSprites.objects[i]->posy );
			}
			else if( pLevelData_Layer1->BasicSprites.objects[i]->type == SPRITE_TYPE_TOPSOLID )
			{
				sprintf(row,  "Sprite %s %d %d HALFMASSIVE\n", image_filename.c_str(), (int) pLevelData_Layer1->BasicSprites.objects[i]->posx, (int)window.h - (int)pLevelData_Layer1->BasicSprites.objects[i]->posy );
			}
			else
			{
				DEBUGLOG( "Warning Level Saving : Unknown Map Object type : %d\n",pLevelData_Layer1->BasicSprites.objects[i]->type );
				continue;
			}
			
			ofs->write( row, strlen( row ) );
		}
	}
}

void cLevel :: SaveMapEnemies()
{
	sprintf( row, "### Enemies ###\n" );
	ofs->write( row, strlen( row ) );
	
	// Enemies
	if( Enemies.objects )
	{
		for( register unsigned int i = 0; Enemies.objcount > i; i++ )
		{
			if( !Enemies.objects[i] )
			{
				continue;
			}
			
			if( Enemies.objects[i]->Enemy_type == ENEMY_AF373 )
			{
				sprintf( row,  "Enemy %d %d %d\n", (int)Enemies.objects[i]->Startposx, (int)Enemies.objects[i]->Startposy, Enemies.objects[i]->Enemy_type );
				ofs->write( row, strlen( row ) );
			}
			else
			{
				DEBUGLOG( "Level Saving : Wrong Enemy type : %d\n", Enemies.objects[i]->Enemy_type );
			}
		}
	}
}

void cLevel :: Update( void )
{
	if( !Mix_PlayingMusic() )
	{
		string filename = DIR_MUSIC + Musicfile;

#ifndef DEMO
		pAudio->PlayMusik( (char *)filename.c_str(),0,SDL_TRUE );
#endif
	}
}

void cLevel :: Draw( void )
{
	// For all tiles Update and Draw... Hm don'chu think we should make a seperate Update & Draw Routine ???

	// I'll tell you why we do it all at once... So we only Loop once>!! It's costly to do it twice...
	// it doesn't seem right.

	/* We should be drawing like 5 layers at a time... So hmmmmmm.... .What does that mean??? 

	This seems to be very dependent on the level at the time it goes aT .

	So i Suppose insteadd of... Having.. ... ..... >  I HAVEN O F >> >> >>> 

	 */ 



	for( register unsigned int i = 0; i < pLevelData_Layer1->BasicSprites.objcount;i++ )
	{
		if( !pLevelData_Layer1->BasicSprites.objects[i] ) 
		{
			continue;
		}

		pLevelData_Layer1->BasicSprites.objects[i]->Update();
		pLevelData_Layer1->BasicSprites.objects[i]->Draw( Renderer );
	}
	
	/////// BRIGHTNESS FADER EFFECT TEST ///////
	/*static Uint8 r=0,g=0,b=0;
	static Uint8 p=0xff;
	
	if (r == 254 || r == 0)
	{
		p = (p^0xFF)+1;
	}
	
	r += p;
	g += p;
	b += p;
	
	
	SDL_SetRenderDrawColor(Renderer, r, g, b, 255);*/
	/////// BRIGHTNESS FADER EFFECT TEST ///////
}

void cLevel :: Parse( char* command, int line )
{
	if ( strlen( command ) <= 5 || *command == '#')
	{
		return;
	}
	
	char* str = command;
	int count = 0;
	int i = 0;
	
	str += strspn(str," ");
	
	while (*str)
	{
		count++;
		str += strcspn(str," ");
		str += strspn(str," ");
	}
	
	str = command; // reset
	
	char** parts = new char*[ count ];
	
	str += strspn(str," ");
	
	while(*str)
	{
		size_t len = strcspn( str," " );
		parts[i] = (char*)SDL_malloc( len + 1 );
		memcpy( parts[i], str, len );
		parts[i][len] = 0;
		str += len + strspn( str + len, " " );
        
        if (parts[i][len-1] == '\r')
            parts[i][len-1] = 0;
        
		i++;
        
        
	}
	
	// Level Message handler
	ParseLine( parts, count, line );
	
	/*for( i = 0; i < count ; i++ )
	{
		//delete( parts[i] );
	}*/
	
	delete []parts;
}

int cLevel :: ParseLine( char ** parts, unsigned int count, unsigned int line )
{
	if ( strcmp( parts[0], "Sprite") == 0 )
	{
		if( count != 5 )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG(" Sprite needs 5 parameters\n");
			return 0; // error
		}

		string full_filename = DIR_PIXMAPS;
		full_filename += parts[1];

		if( !FileValid( full_filename ) )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG(" Sprite image does not exist\n");
			return 0; // error
		}

		if( !is_valid_number( parts[2] ) )
		{
			DEBUGLOG( "%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid integer value\n", parts[1] );
			return 0; // error
		}
		if( !is_valid_number( parts[3] ) )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid integer value\n", parts[1] );
			return 0; // error
		}

		IMan->Add( LoadImage( full_filename.c_str(), colorkey ), full_filename );

		if( strcmp( parts[4], "SOLID" ) == 0 )
		{
			cMVelSprite *temp = new cMVelSprite( IMan->GetPointer( full_filename ), (double)atoi( parts[2] ), (double)window.h -(double)atoi( parts[3] ) );
			temp->type = SPRITE_TYPE_SOLID;
			pLevelData_Layer1->AddSprite( temp );
		}
		else if( strcmp( parts[4], "PASSIVE" ) == 0 )
		{
			cMVelSprite *temp = new cMVelSprite( IMan->GetPointer( full_filename ), (double)atoi( parts[2] ), (double)window.h -(double)atoi( parts[3] ) );
			temp->type = SPRITE_TYPE_PASSIVE;
			pLevelData_Layer1->AddSprite( temp );
		}
		else if( strcmp( parts[4], "TOPSOLID" ) == 0 )
		{
			cMVelSprite *temp = new cMVelSprite( IMan->GetPointer( full_filename ), (double)atoi( parts[2] ), (double)window.h - (double)atoi( parts[3] ) );
			temp->type = SPRITE_TYPE_TOPSOLID;
			pLevelData_Layer1->AddSprite( temp );
		}
		//else if ( strcmp( parts ) )
		else if( strcmp( parts[4], "BOTTOMSOLID" ) == 0 )
		{
			cMVelSprite *temp = new cMVelSprite( IMan->GetPointer( full_filename ), (double)atoi( parts[2] ), (double)window.h - (double)atoi( parts[3] ) );
			temp->type = SPRITE_TYPE_BOTTOMSOLID;
			pLevelData_Layer1->AddSprite( temp );
		}
		else if( strcmp( parts[4], "LEFTSOLID" ) == 0 )
		{
			cMVelSprite *temp = new cMVelSprite( IMan->GetPointer( full_filename ), (double)atoi( parts[2] ), (double)window.h - (double)atoi( parts[3] ) );
			temp->type = SPRITE_TYPE_LEFTSOLID;
			pLevelData_Layer1->AddSprite( temp );
		}
		else if( strcmp( parts[4], "RIGHTSOLID" ) == 0 )
		{
			cMVelSprite *temp = new cMVelSprite( IMan->GetPointer( full_filename ), (double)atoi( parts[2] ), (double)window.h - (double)atoi( parts[3] ) );
			temp->type = SPRITE_TYPE_RIGHTSOLID;
			pLevelData_Layer1->AddSprite( temp );
		}
		else
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid Sprite type\n", parts[4] );
			return 0; // error
		}
		
	}
	else if( strcmp( parts[0], "Enemy") == 0 )
	{
		if( count != 4 )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG(" Enemy needs 4 parameters\n");
			return 0; // error
		}

		if( !is_valid_number( parts[1] ) )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid integer value\n", parts[1] );
			return 0; // error
		}
		
		if( !is_valid_number( parts[2] ) )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid integer value\n", parts[1] );
			return 0; // error
		}
		
		if( !is_valid_number( parts[3] ) )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid integer value\n", parts[1] );
			return 0; // error
		}

		if( atoi( parts[3] ) == ENEMY_AF373 )
		{
			AddEnemy( atoi( parts[1] ), atoi( parts[2] ), atoi( parts[3] ) );
		}
		else
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid Enemy type\n", parts[3] );
			return 0; // error
		}
	}
	else if( strcmp( parts[0], "Player") == 0 )
	{
		if( count != 3 )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG(" Player needs 3 parameters\n");
			return 0; // error
		}
		if( !is_valid_number( parts[1] ) )
		{
			DEBUGLOG( "%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid integer value\n", parts[1] );
			return 0; // error
		}
		if( !is_valid_number( parts[2] ) )
		{
			DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s is not a valid integer value\n", parts[1] );
			return 0; // error
		}

		pPlayer->SetPos( (double) atoi( parts[1] ), /*(double)window.h - */(double) atoi( parts[2] ) );
		
		pPlayer->Startposx = (double) atoi( parts[1] );
		pPlayer->Startposy = /*(double)window.h - */(double) atoi( parts[2] );
	}
	else if( strcmp( parts[0], "BGColor") == 0 )
	{
		Uint8 r,g,b;
		r = (Uint8)atoi(parts[1]);
		g = (Uint8)atoi(parts[2]);
		b = (Uint8)atoi(parts[3]);
		SDL_SetRenderDrawColor(Renderer, r, g, b, 255);
	}
	else if( strcmp( parts[0], "Music") == 0 )
	{
		string filename = DIR_MUSIC;
		filename += parts[1];

		if( !FileValid( filename ) ) 
		{
			DEBUGLOG( "%s : line %d Error : ", levelfile.c_str() , line );
			DEBUGLOG( "%s music file does not exist\n", parts[1] );
			return 0; // error			
		}

		Musicfile = parts[1];
	}
	else
	{
		DEBUGLOG("%s : line %d Error : ", levelfile.c_str() , line );
		DEBUGLOG(" Unknown definition : %s\n", parts[0] );
		return 0; // error
	}

	return 1; // Succesfull	
}
