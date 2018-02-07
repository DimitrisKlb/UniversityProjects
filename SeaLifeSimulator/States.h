#ifndef STATES_H
#define STATES_H
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "SurfaceLib.h"
#include "GraphicDefines.h"

//#include "Simulator.h"

class Simulator;
enum{
	STATE_INTRO,
	STATE_SIMULATION,
	STATE_HELP
};
class State{
public:
	State();
	virtual bool Init(Simulator* sim,int=0,int=0)=0;
	virtual void EventHandle(SDL_Event* Event)=0;
	virtual void Loop() =0;
	virtual void Render(SDL_Surface* surfDisplay) =0;
	virtual void CleanUp()=0;

	void SetSimulator(Simulator*);
	Simulator* GetSimulator();

protected:
	Simulator* theSim;
};


#endif