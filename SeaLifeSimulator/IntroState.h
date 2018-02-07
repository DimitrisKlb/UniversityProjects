#ifndef INTRO_STATE_H
#define INTRO_STATE_H
#include "States.h"
# include  "MyTextField.h"

class IntroState : public State{
public:
	IntroState();
	bool Init(Simulator* sim,int=0,int=0);
	void EventHandle(SDL_Event* Event);
	void Loop();
	void Render(SDL_Surface* surfDisplay);
	void CleanUp();

	static IntroState* GetInstance();
private:
	static IntroState singleInstance;

	SDL_Surface* menuSurf;
	My_SDL_TextField* textField1;
	My_SDL_TextField* textField2;

public:
	int seaY,seaX;
};

#endif