#ifndef HELP_STATE_H
#define HELP_STATE_H
#include "States.h"

class HelpState : public State{
public:
	HelpState();
	bool Init(Simulator* sim,int=0,int=0);
	void EventHandle(SDL_Event* Event);
	void Loop();
	void Render(SDL_Surface* surfDisplay);
	void CleanUp();

	static HelpState* GetInstance();
private:
	static HelpState singleInstance;	

	SDL_Surface* helpSurf;
};

#endif