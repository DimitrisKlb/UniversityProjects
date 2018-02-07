#include "HelpState.h"
#include "Simulator.h"

HelpState HelpState::singleInstance;
HelpState* HelpState::GetInstance(){ return &singleInstance;}

HelpState::HelpState(){
	helpSurf = NULL;
}
bool HelpState::Init(Simulator* sim,int sy,int sx){
	SetSimulator(sim);
	if( (helpSurf = SurfaceLib::LoadImg("images/help.png")) == NULL) {
		return false;
	}
	return true;
}
void HelpState::EventHandle(SDL_Event* Event){
	if(Event->type == SDL_QUIT) {
        theSim->SetRunning(false);
	}else if(Event->type==SDL_KEYDOWN){
		switch(Event->key.keysym.sym) {
		case SDLK_ESCAPE:
			theSim->ChangeState(STATE_SIMULATION);
			break;
		}
	}
}
void HelpState::Loop(){

}
void HelpState::Render(SDL_Surface* screen){
	SurfaceLib::Draw(screen,helpSurf,0,0);
	SDL_Flip(screen);
}
void HelpState::CleanUp(){
	SDL_FreeSurface(helpSurf);
}