// Basiki klasi grafikwn pou periexei to Game-Loop kai Dimiourgia tou para8irou

#ifndef SIMULATOR_H
	 #define SIMULATOR_H
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "SurfaceLib.h"
#include "GraphicDefines.h"

#include "States.h"
#include "IntroState.h"
#include "HelpState.h"
#include "SimulationState.h"

class Simulator{
public:
	Simulator(char* wName,int wHeight,int wWidth);
	
	int Execute();
	bool Init();
	void EventHandle(SDL_Event* Event);
	void Loop();
	void Render();
	void Cleanup();

	void ChangeState(int nextState);
	void SetRunning(bool);
	char* GetWinName();
	State** GetStates();
private:
	char* winName;            // Titlos para8irou
	int winHeight,winWidth;   // Diastaseis para8irou
	bool running;             // Dikniousa tou an i efarmogi prepei na sinexisei na trexei
	SDL_Surface* screen;      // Basiki epifaneia-panel tou para8irou

	State** theStates;
	int activeState;
};





#endif