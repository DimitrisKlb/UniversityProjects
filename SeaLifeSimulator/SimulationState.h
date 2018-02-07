#ifndef SIMULATION_STATE_H
	 #define SIMULATION_STATE_H

#include "States.h"
#include "CameraLib.h"
#include "MyButton.h"
#include  "MyTextField.h"
#include "Sea.h"

enum{
	MODE_NORMAL,
	MODE_ADDORG,
	MODE_ADDCONTAM,
	MODE_NETS
};
class SimulationState : public State{
public:
	SimulationState();
	bool Init(Simulator* sim,int=0,int=0);
	void EventHandle(SDL_Event* Event);
	void Loop();
	void Render(SDL_Surface* surfDisplay);
	void CleanUp();
	
	static SimulationState* GetInstance();
	void Restart();
private: 
	inline void RenderTop(SDL_Surface* surfDisplay);
	inline void RenderMap(SDL_Surface* surfDisplay);
	inline void RenderMisc(SDL_Surface* surfDisplay);
	inline void RenderMiscTop(SDL_Surface* surfDisplay);
	inline void RenderMiscBot(SDL_Surface* surfDisplay);

	inline void Pause();
	inline void ChangeMode( int tmode);
	inline bool hasNets(int y,int x);
private:
	static SimulationState singleInstance;

	int seaY,seaX;
	Sea* theSea;
	CameraHandler* camera;	
	long unsigned int lastUpdate;
	unsigned int timeDelay;
	bool paused;
	int mode;
	Organism* selected;
	int selectedClass;
	Point net1,net2;
	bool firstTime;

	SDL_Surface* mapSurf;
	SDL_Surface* miscSurf;
	SDL_Surface* topSurf;
	SDL_Surface* fishes;
	SDL_Surface* tiles;	
	SDL_Surface* nets;
	SDL_Surface* dateSurf;
	SDL_Surface* infoMsg;	
	SDL_Surface* fishInfo;
	SDL_Surface* classInfo;	

	TTF_Font* dateFont;
	TTF_Font* infoFont;
	TTF_Font* numsFont;

	My_SDL_Button* buttonPlus;
	My_SDL_Button* buttonContam;
	My_SDL_Button* buttonNets;
	My_SDL_Button* buttonHelp;
	My_SDL_TextField* newField;
	
};


#endif