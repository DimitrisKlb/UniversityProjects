#include "MyButton.h"

My_SDL_Button::My_SDL_Button(int ty,int tx,int theight,int twidth){
	y = ty;
	x = tx;
	height = theight;
	width = twidth;

	buttonSurf = NULL;
	
	pushed = false;
}
My_SDL_Button::~My_SDL_Button(){
	SDL_FreeSurface(buttonSurf);
}

void My_SDL_Button::setPushed(bool tpushed){
	pushed = tpushed;
}
bool My_SDL_Button::isPushed(){return pushed;}

bool My_SDL_Button::LoadImage(char* imageName){
	buttonSurf = SurfaceLib::LoadImg(imageName);
	if( buttonSurf == NULL) return false;
	return true;
}
bool My_SDL_Button::Belongs(int yClicked,int xClicked){
	if( (yClicked > y)&&(yClicked < y+height)&&(xClicked > x)&&(xClicked < x+width) ){
		return true;
	}
	return false;
}

void My_SDL_Button::Render(SDL_Surface* container){
	if(container==NULL) return;
	if(!pushed)
		SurfaceLib::Draw(container,buttonSurf,x,y,0,0,width,height);
	else
		SurfaceLib::Draw(container,buttonSurf,x,y,width,0,width,height);
}