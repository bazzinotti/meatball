#include "TextObject.h"

extern SDL_Renderer *Renderer;

void TextObject::SetBGColor(SDL_Color color)
{
	bgColor = color;
}
void TextObject::SetFGColor(SDL_Color color)
{
	fgColor = color;
}

void TextObject::SetColors(SDL_Color fg, SDL_Color bg)
{
	fgColor = fg;
	bgColor = bg;
}

void TextObject::initDefaultColors()
{
	bgColor = SetColor( 0, 0, 100 );	// Default Background Color : Dark Blue
	fgColor = SetColor( 255, 255, 255 );	// Default Text Color : White
}

void TextObject::preRender()
{
	freeSurface();
	freeTexture();
}
/// Makes the surface and updates the Rect
void TextObject::Render()
{
	preRender();
	surface = cFont::CreateText(text.c_str(), font,FONT_TYPE_BLENDED,fgColor,bgColor);
	texture = SDL_CreateTextureFromSurface(Renderer, surface);
	UpdateRect();
}

void TextObject::Render_blended()
{
	preRender();
	surface = cFont::CreateText(text.c_str(), font,FONT_TYPE_BLENDED,fgColor,bgColor);
	texture = SDL_CreateTextureFromSurface(Renderer, surface);
	UpdateRect();
}

void TextObject::Render_shaded()
{
	preRender();
	surface = cFont::CreateText(text.c_str(), font,FONT_TYPE_SHADED,fgColor,bgColor);
	texture = SDL_CreateTextureFromSurface(Renderer, surface);
	UpdateRect();
}

void TextObject::Render_solid()
{
	preRender();
	surface = cFont::CreateText(text.c_str(), font,FONT_TYPE_SOLID,fgColor,bgColor);
	texture = SDL_CreateTextureFromSurface(Renderer, surface);
	UpdateRect();
}


void TextObject::Render(TTF_Font *font)
{
	preRender();
	Render_blended();
	UpdateRect();
}

void TextObject::Draw(SDL_Renderer *renderer)
{
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void TextObject::Draw()
{
	Draw(Renderer);
}

/// Update the surface with the object's internal text and font
TextObject::TextObject()
{
	rendertext = SDL_FALSE;
	font = NULL;
	text = "";
	surface = NULL;
	texture = NULL;
	initDefaultColors();
}

TextObject::TextObject(string text)
{
	rendertext = SDL_FALSE;
	initDefaultColors();
	this->text = text;	
}

/// Write text with a different font
/// This won't change the text object's default font
void TextObject::RenderDifferentFont(TTF_Font *different_font)
{
	Render(different_font);
}

TextObject::~TextObject()
{
	// nothing
}

/// Update's the surface and internal font and text members
TextObject::TextObject(string text, TTF_Font *font)
{
	rendertext = SDL_FALSE;
	initDefaultColors();
	this->font = font;
	this->text = text;
}

TextObject::TextObject(int x, int y)
{
	rendertext = SDL_FALSE;
	initDefaultColors();
	SetPos(x,y);
}

TextObject::TextObject(int x, int y, string text)
{
	rendertext = SDL_FALSE;
	initDefaultColors();
	this->text = text;
	SetPos(x,y);
}

TextObject::TextObject(int x, int y, string text, TTF_Font *font)
{
	initDefaultColors();
	this->font = font;
	this->text = text;
	SetPos(x,y);
	rendertext = SDL_FALSE;
}


void TextObject::UpdateRect(int x, int y)
{
	rect = SetRect(x,y,surface->w, surface->h);
}

void TextObject::UpdateRect()
{
	rect = SetRect(rect.x, rect.y, surface->w, surface->h);
}


void TextObject::SetPos(int x, int y)
{
	rect.x = x;
	rect.y = y;
}

void TextObject::Update()
{
	if (rendertext)
	{
		rendertext = SDL_FALSE;
		
		Render();
		
		UpdateRect();
	}
}