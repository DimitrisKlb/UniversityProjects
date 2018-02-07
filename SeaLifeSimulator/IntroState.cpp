#include "IntroState.h"
#include "Simulator.h"

IntroState IntroState::singleInstance;
IntroState* IntroState::GetInstance(){ return &singleInstance;}

IntroState::IntroState(){
	menuSurf = NULL;
	textField1 = new My_SDL_TextField(260,305,20,130);
	textField2 = new My_SDL_TextField(260,475,20,130);
}
bool IntroState::Init(Simulator* sim,int sy,int sx){
	SetSimulator(sim);
	if( (menuSurf = SurfaceLib::LoadImg("images/intro.png")) == NULL) {
		return false;
	}
	if( textField1->Load("images/text1.png","fonts/calibri.ttf",20) == false ){
		return false;
	}
	if( textField2->Load("images/text2.png","fonts/calibri.ttf",20) == false ){
		return false;
	}
	return true;
}
void IntroState::EventHandle(SDL_Event* Event){
	if(Event->type == SDL_QUIT) {
        theSim->SetRunning(false);
	}else if(Event->type==SDL_KEYDOWN){
		switch(Event->key.keysym.sym) {
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			seaX = textField1->GetNumber();
			seaY = textField2->GetNumber();
			

			if( theSim->GetStates()[STATE_SIMULATION]->Init(theSim,seaY,seaX) == false){
				theSim->SetRunning(false);
			}
			theSim->ChangeState(STATE_SIMULATION);
			break;
		
		}
		if(textField1->isFocused()){
			textField1->AddText(Event);
		}else if(textField2->isFocused()){
			textField2->AddText(Event);
		}
	}else if(Event->type == SDL_MOUSEBUTTONDOWN){
		int yClicked = Event->button.y;
		int xClicked = Event->button.x;		
		switch(Event->button.button){		
		case SDL_BUTTON_LEFT:
			if(textField1->Belongs(yClicked,xClicked)){
				textField1->setFocused(true);
				textField2->setFocused(false);
			}else if(textField2->Belongs(yClicked,xClicked)){
				textField2->setFocused(true);
				textField1->setFocused(false);
			}else{
				textField1->setFocused(false);
				textField2->setFocused(false);
			}
			break;
		}	
	}
}
void IntroState::Loop(){

}
void IntroState::Render(SDL_Surface* screen){
	SurfaceLib::Draw(screen,menuSurf,0,0);
	textField1->Render(screen);
	textField2->Render(screen);
	SDL_Flip(screen);
}
void IntroState::CleanUp(){
	SDL_FreeSurface(menuSurf);
	delete textField1;
	delete textField2;
}