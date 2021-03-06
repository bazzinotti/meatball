

#include "SDL_ep.h"

using namespace std;

cAudio :: cAudio( SDL_bool playMusic /* = 1 */, SDL_bool playSounds /* = 1 */, 
				 Uint8 MusicVol /* = 80 */, Uint8 SoundVol /* = 110  */)
{
	Music = NULL;
	
	Sound_Volume = SoundVol;
	Music_Volume = MusicVol;
	Sounds_Played = 0; 
	Music_Played = 0;
	bSounds = playSounds;
	bMusic = playMusic;
	bDebug = SDL_FALSE;
	bInitialised = SDL_FALSE;

	// Usually 22.050 , 44.100, 44.800
	iHz = 44100;
	// Usually 1024 , 2048 , 4096
	iBuffer = 4096;
    // 1 = Mono , 2 = Stereo
	iChannels = MIX_DEFAULT_CHANNELS; 
}


cAudio :: ~cAudio( void )
{
	HaltSounds();

	if( bMusic == SDL_TRUE && Music)
	{
		StopMusic();
		Mix_FreeMusic( Music );
		Music = NULL;
	}

	if( bInitialised == SDL_TRUE )
	{
		Mix_CloseAudio();
	}
}

void cAudio :: InitAudio( void )
{
	if (( bMusic == SDL_TRUE || bSounds == SDL_TRUE ) && bInitialised == SDL_FALSE )
	{
		if( bDebug == SDL_TRUE )
		{
			cout << "Initializing Audio System ,Buffer = " << iBuffer << "  Frequency = " << iHz <<'\n' << flush;
		}

		/** Initialising prefered Audio System specs with Mixer Standard format (Stereo)
		*
		*	format		: Output sample format.
		*	channels	: Number of sound channels in output.
		*	Set to 2 for stereo, 1 for mono. This
		*	chunksize	: Bytes used per output sample.
		*/
		if ( Mix_OpenAudio( iHz, MIX_DEFAULT_FORMAT , iChannels, iBuffer) < 0 ) 
		{
			fprintf( stderr, "Warning : Could not init 16-bit Audio\n- Reason : %s\n",SDL_GetError() );
		}
		else
		{
			bInitialised = SDL_TRUE;
			SetMusicVolume( Music_Volume );
			SetAllSoundsVolume( Sound_Volume );
		}
	}
	else
	{
		if( bDebug == SDL_TRUE )
		{
			printf( "Audio System will not be Initialized" );
		}	
	}
}

void cAudio :: ReserveChannels( int num )
{
	int gnum = Mix_ReserveChannels( num );

	if( ( gnum != num ) && bDebug ) // Warning
	{
		printf( "Audio Warning ReserveChannels : Wanted : %d , got : %d !\n", num, gnum );
	}
}

void cAudio :: AllocateChannels( int num )
{
	int gnum = Mix_AllocateChannels( num ); // FIX : ??-??

	if( ( gnum != num ) && bDebug ) // Warning
	{
		printf( "Audio Warning AllocateChannels : Wanted : %d , got : %d !\n", num, gnum );
	}

	//iChannels = gnum;
}

Mix_Chunk *cAudio :: LoadSound( const char *filename, int sVolume /* = -2  */ )
{
	if( !filename ) 
	{
		printf( "LoadSound Error : filename is empty\n" );
		return NULL;
	}

	if( bSounds == SDL_FALSE || bInitialised == SDL_FALSE )
	{
		return NULL;
	}
		
	Mix_Chunk *Sound = Mix_LoadWAV( filename );
	

	if( !Sound ) 
	{
		printf( "LoadSound Error : could not load sound file : %s\n", filename );
		return NULL;
	}

	if( sVolume > 0 )
	{
		if( sVolume > 128 )
		{
			printf("LoadSound Warning : Volume is out of range : %d\n", sVolume );
		}

		Mix_VolumeChunk( Sound, sVolume );
	}
	else if( sVolume == -1 )
	{
		Mix_VolumeChunk( Sound, Sound_Volume );
	}
	
	return Sound;
}

int cAudio :: PlaySound( Mix_Chunk *sound, int channel /* = -1 */, int sVolume /* = -1  */ )
{
	if( !sound ) 
	{
		printf( "PlaySound Error : sound is not allocated\n" );
		return -1;
	}

	if( channel > 128 )
	{
		printf( "PlaySound Error : channel is out of range : %d\n", channel );
		return -1;
	}
	
	if ( bSounds == SDL_FALSE || bInitialised == SDL_FALSE)
	{
		return -1;
	}
	int channel_used = Mix_PlayChannel( channel, sound, 0 );

	if( channel_used == -1 && bDebug )
	{
		printf( "PlaySound Error : could not play sound \n" );
   		return -1;
	}
	else
	{
		Sounds_Played++;

		if( sVolume > 0 )
		{
			if( sVolume > 128 )
			{
				printf("PlaySound Warning : Volume is out of range : %d\n", sVolume );
			}

			Mix_Volume( channel_used, sVolume );
		}
		else if( sVolume == -1 )
		{
			Mix_Volume( channel_used, Sound_Volume );
		}
	}

	return channel_used;
}

void cAudio:: LoadMusic(const char *filename)
{
	if ( bMusic == SDL_TRUE && bInitialised == SDL_TRUE )
	{
		Music = Mix_LoadMUS( filename );
	}
}

void cAudio :: PlayMusik( const char *filename, int loops /* = -1 */, SDL_bool force /* = 1  */ )
{
	if ( bMusic == SDL_TRUE && bInitialised == SDL_TRUE )
	{
		if ( !Mix_PlayingMusic() || force == SDL_TRUE )
		{
			if( Music )
			{
				Mix_HaltMusic();
				Mix_FreeMusic( Music );
				Music = NULL;
			}

			// Loads the Music file
			Music = Mix_LoadMUS( filename );
			Music_Played++;
			Mix_PlayMusic( Music, loops );
		}
		else
		{
			Music = Mix_LoadMUS( filename );
		}
	}
}

void cAudio :: StopMusic( void ) 
{
	if( Mix_PlayingMusic() )
	{
		Mix_HaltMusic();
	}		
}

void cAudio :: FadeOutMusic( unsigned int ms )
{
	if( Mix_PlayingMusic() ) // Check if music is currently playing
	{
		Mix_FadeOutMusic( ms );
	}
}

// Pauses playing Music
void cAudio :: PauseMusic( void )
{
	if( Mix_PlayingMusic() )
	{
		Mix_PauseMusic();
	}
}

void cAudio :: ResumeMusic( void )
{
	Mix_ResumeMusic();
}

void cAudio :: HaltSounds( int channel /* = -1  */ )
{
	Mix_HaltChannel( channel );
}

void cAudio :: PauseSounds( int channel /* = -1  */ )
{
	// Pauses all Channels
	Mix_Pause( channel );
}

void cAudio :: ResumeSounds( int channel /* = -1  */ )
{
	// resume playback on all previously active channels
	Mix_Resume( channel );
}

void cAudio :: FadeOutSounds( int milliseconds /* = 2000 */, int channel /* = -1  */ )
{
	Mix_FadeOutChannel( channel, milliseconds );
}

int cAudio :: GetSoundVolume(Mix_Chunk *chunk)
{
	// dont fuck with a NULL chunk ;P 
	if (chunk == NULL)
	{
		return -1;
	}
	
	// Pass negative volume value to get current Volume and not change it
	return Mix_VolumeChunk(chunk, -1);
}


int cAudio :: SetSoundVolume (Mix_Chunk *chunk, int iVolume)
{
	if( iVolume > 128 || chunk == NULL)
	{
		return -1;
	}

	return Mix_VolumeChunk(chunk, iVolume);
}


/** Doesn't really Work :| */
int cAudio :: SetAllSoundsVolume( Uint8 iVolume )
{
	if( iVolume > 128 || iVolume < 0 )
	{
		return -1;
	}

	Sound_Volume = iVolume;

	return Mix_Volume( -1, iVolume );
}

int cAudio :: SetMusicVolume( Uint8 iVolume )
{
	if( iVolume > 128 || iVolume < 0 )
	{
		return -1;
	}

	Music_Volume = iVolume;

	return Mix_VolumeMusic( iVolume );

	
}

SDL_bool cAudio :: MusicPlaying()
{
	return Mix_PlayingMusic() ? SDL_TRUE : SDL_FALSE;
}

void cAudio :: Update( void )
{
	if ( bMusic == SDL_TRUE )
	{
		if ( !Mix_PlayingMusic() ) 
		{
			Music_Played++;
			Mix_PlayMusic( Music, 0 );
		}
	}
}