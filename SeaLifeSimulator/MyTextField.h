#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include <sstream>
#include <cstdlib>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "SurfaceLib.h"
using std::stringstream;

class My_SDL_TextField{
private:
	int y,x;
	int height,width;
	bool focused;	
	stringstream textStream;
	unsigned int maxDigits;

	SDL_Surface* textSurf;
	TTF_Font* textFont;
public:
	My_SDL_TextField(int ty=0,int tx=0,int theight=0,int twidth=0);
	~My_SDL_TextField();

	void setFocused(bool tfocused);
	bool isFocused();

	void AddText(SDL_Event* keyEve);
	int GetNumber();
	
	bool Load(char* imageName,char* fontName,int fontSize);
	bool Belongs(int yClicked,int xClicked);
	void Render(SDL_Surface* container);
};

#endif
