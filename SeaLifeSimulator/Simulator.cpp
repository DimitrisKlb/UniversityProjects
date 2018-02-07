#include "Simulator.h"

Simulator::Simulator(char* wName,int wHeight,int wWidth){
	winName = wName;
	winHeight = wHeight;
	winWidth = wWidth;
	running = true;
	screen = NULL;	

	theStates = new State*[3];
	activeState = STATE_INTRO;
	theStates[0] = IntroState::GetInstance();
	theStates[1] = SimulationState::GetInstance();
	theStates[2] = HelpState::GetInstance();
}
int Simulator::Execute() {
    if(Init() == false) {
        return -1;
    }

    SDL_Event Event;

    while(running) {
        while(SDL_PollEvent(&Event)) {
            EventHandle(&Event);
        }

        Loop();
        Render();
    }
    Cleanup();
	return 0;
}
bool Simulator::Init(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }
	if (TTF_Init() < 0) {
		return false;
	}
	SDL_WM_SetIcon(IMG_Load("images/iconfish.ico"), NULL);
	SDL_WM_SetCaption(winName,"default");

    if((screen = SDL_SetVideoMode(winWidth,winHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
        return false;
    }	
	if( !theStates[STATE_INTRO]->Init(this) )
		return false;
	if( !theStates[STATE_HELP]->Init(this) )
		return false;	

	SDL_EnableKeyRepeat(1, SDL_DEFAULT_REPEAT_INTERVAL*5);
	return true;
}
void Simulator::EventHandle(SDL_Event* Event){
		theStates[activeState]->EventHandle(Event);
}	
void Simulator::Loop(){	
	theStates[activeState]->Loop();
}
void Simulator::Render(){
	theStates[activeState]->Render(screen);
}
void Simulator::Cleanup(){
	SDL_FreeSurface(screen);
	theStates[STATE_INTRO]->CleanUp();
	theStates[STATE_SIMULATION]->CleanUp();
	theStates[STATE_HELP]->CleanUp();
	TTF_Quit();
	SDL_Quit();
}

void Simulator::ChangeState(int nextState){	
	activeState = nextState;
}
void Simulator::SetRunning(bool run){
	running = run;
}
char* Simulator::GetWinName(){
	return winName;
}

State** Simulator::GetStates(){
	return theStates;
}