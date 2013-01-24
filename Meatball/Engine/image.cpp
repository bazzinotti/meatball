

#include "SDL_ep.h"

SDL_Surface *LoadImage( const char *szFilename, Uint32 Colorkey /* = 0 */, Uint8 Alpha /* = 255  */ )
{
	FILE* fp = fopen( szFilename, "r" );

	if ( !fp )
	{
		printf( "Error loading file failed : %s\n", szFilename );
		return NULL;
	} 
	else
	{
		fclose( fp );
	}

	SDL_Surface *image, *temp;
	
	temp = IMG_Load( szFilename );

	if ( !temp )
	{
		printf("Error loading file : %s\n",szFilename);
		return NULL;
	}
	
	SDL_SetColorKey( temp, SDL_SRCCOLORKEY | SDL_RLEACCEL | SDL_SRCALPHA, Colorkey );
	SDL_SetAlpha( temp, SDL_SRCCOLORKEY | SDL_RLEACCEL | SDL_SRCALPHA, Alpha );

	image = SDL_DisplayFormat( temp );

	SDL_FreeSurface( temp );

	return image;
}

SDL_Surface *CreateSurface( unsigned int width, unsigned int height, Uint8 bpp /* = 32 */, 
			Uint32 flags /* = SDL_SWSURFACE | SDL_SRCCOLORKEY | SDL_RLEACCEL | SDL_SRCALPHA  */)
{
	if( !width || !height ) 
	{
		printf("CreateRGBSurface Error : 0 dimension specified\n");
		return NULL;
	}

    Uint32 rmask, gmask, bmask, amask;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	#endif

	SDL_Surface *image, *temp;

	temp = SDL_CreateRGBSurface( flags, width, height, bpp, rmask, gmask, bmask, amask );

	image = SDL_DisplayFormat( temp );

	SDL_FreeSurface( temp );
	
	if( !image ) 
	{
        printf( "CreateRGBSurface failed : %s\n", SDL_GetError() );
    }
	
	return image;
}