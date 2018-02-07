#ifndef MY_BUTTON_H
	 #define MY_BUTTON_H

#include <SDL.h>
#include <SDL_image.h>
#include "SurfaceLib.h"

class My_SDL_Button{
private:
	int y,x;
	int height,width;
	SDL_Surface* buttonSurf;
	bool pushed;
public:
	My_SDL_Button(int ty=0,int tx=0,int theight=0,int twidth=0);
	~My_SDL_Button();

	void setPushed(bool tpushed);
	bool isPushed();

	bool LoadImage(char* imageName);
	bool Belongs(int yClicked,int xClicked);
	void Render(SDL_Surface* container);

};

#endif