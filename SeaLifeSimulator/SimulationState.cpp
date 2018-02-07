#include <sstream>
#include <iomanip>

#include "SimulationState.h"
#include "Simulator.h"

SimulationState SimulationState::singleInstance;
SimulationState* SimulationState::GetInstance(){ return &singleInstance;}


SimulationState::SimulationState(){
	seaY = seaX = 50;
	theSea = NULL;
	camera = NULL;	
	lastUpdate = 0;
	timeDelay = 1000;
	paused = false;
	mode = MODE_NORMAL;
	selected = NULL;
	selectedClass = -1;
	net1.x = net1.y = net2.x = net2.y = -1;
	firstTime = true;
	
	mapSurf = NULL;
	miscSurf = NULL;
	topSurf = NULL;
	fishes = NULL;
	tiles = NULL;
	nets = NULL;
	dateSurf = NULL;
	infoMsg = NULL;
	fishInfo = NULL;
	classInfo = NULL;
	
	dateFont = NULL;
	infoFont = NULL;
	numsFont = NULL;
	
	buttonPlus = new My_SDL_Button(22,463,26,26);
	buttonContam = new My_SDL_Button(22,491,26,26);
	buttonNets = new My_SDL_Button(22,519,26,26); 
	buttonHelp = new My_SDL_Button(22,547,26,26);
	newField = new My_SDL_TextField(577,773,20,120);
}

bool SimulationState::Init(Simulator* sim,int tseaY,int tseaX){
	SetSimulator(sim);	
	Pause();
	if(tseaY>=11 && tseaY<=1000)
		seaY = tseaY;
	if(tseaX>=11 && tseaX<=1000)
		seaX = tseaX;
	camera = new CameraHandler(seaY,seaX);
	theSea = new Sea(seaY,seaX,true);

	if(firstTime){
		firstTime = false;
		if( (mapSurf = SurfaceLib::LoadImg("images/mapFrame.png")) == NULL) {
			return false;
		}	
		if( (miscSurf = SurfaceLib::LoadImg("images/miscFrame.png")) == NULL) {  
			return false;
		}	
		if( (topSurf = SurfaceLib::LoadImg("images/top.png")) == NULL) {
			return false;
		}
		if( (fishes = SurfaceLib::LoadImg("images/fishes.png")) == NULL) {
			return false;
		}	
		if( (nets = SurfaceLib::LoadImg("images/nets.png")) == NULL) {
			return false;
		}
		if( (tiles = SurfaceLib::LoadImg("images/seaTiles.png")) == NULL) {
			return false;
		}
		if( (fishInfo = SurfaceLib::LoadImg("images/fishInfo.png")) == NULL) {
			return false;
		}
		if( (classInfo = SurfaceLib::LoadImg("images/classInfo.png")) == NULL) {
			return false;
		}
		SurfaceLib::Transparent(fishes,255,0,255);
		SurfaceLib::Transparent(nets,255,0,255);
		SurfaceLib::Transparent(fishInfo,0,0,0);
		SurfaceLib::Transparent(classInfo,0,0,0);

		if( (infoFont = TTF_OpenFont("fonts/calibri.ttf", 18)) == NULL){
			return false;
		}
		if( (dateFont = TTF_OpenFont("fonts/GAGASM__.ttf", 32)) == NULL){
			return false;
		}
		if( (numsFont = TTF_OpenFont("fonts/calibri.ttf", 12)) == NULL){
			return false;
		}
		if( buttonPlus->LoadImage("images/buttonPlus.png") == false ){
			return false;
		}
		if( buttonContam->LoadImage("images/buttonContam.png") == false ){
			return false;
		}
		if( buttonNets->LoadImage("images/buttonNets.png") == false ){
			return false;
		}
		if( buttonHelp->LoadImage("images/buttonHelp.png") == false ){
			return false;
		}
		if( newField->Load("images/text3.png","fonts/calibri.ttf",20) == false ){
			return false;
		}
	}
	return true;
}
void SimulationState::EventHandle(SDL_Event* Event){
	if(Event->type == SDL_QUIT) {
        theSim->SetRunning(false);
	}else if(Event->type==SDL_KEYDOWN){
		switch(Event->key.keysym.sym) {
		case SDLK_LEFT: 
			camera->move(0,-1);
			break;
		case SDLK_RIGHT: 
			camera->move(0,1);
			break;
		case SDLK_UP: 
			camera->move(-1,0);
		    break;
		case SDLK_DOWN: 
			camera->move(1,0);
		    break;
		case SDLK_SPACE:
			
			if(mode!=MODE_NORMAL)
				ChangeMode(MODE_NORMAL);	
			Pause();
			break;
		case SDLK_MINUS:
		case SDLK_KP_MINUS:
			if(timeDelay>=150)
				timeDelay-=50;
			break;
		case SDLK_EQUALS:
		case SDLK_KP_PLUS:
			if(timeDelay<5000)
				timeDelay+=50;
			break;		
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			if(mode==MODE_NETS && net2.y!=-1){
				selected = NULL;
				theSea->throwNets(net1.y,net1.x,net2.y,net2.x);		
				net1.x = net1.y = net2.x = net2.y = -1;
			}
			break;
		case SDLK_ESCAPE:
			if(mode==MODE_NETS)
				net1.x = net1.y = net2.x = net2.y = -1;
			break;
		case SDLK_F1:
			buttonHelp->setPushed(true);
				if(!paused)
					Pause();
			break;
		case SDLK_r:
			Restart();
			break;
		}
		if(newField->isFocused()){
			newField->AddText(Event);
		}
	}else if(Event->type == SDL_MOUSEBUTTONDOWN){
		int yClicked = Event->button.y;
		int xClicked = Event->button.x;		
		int yOrg = (yClicked-50)/TILE_SIZE + camera->getY()-5;
		int xOrg = (xClicked-25)/TILE_SIZE + camera->getX()-5;
	
		switch(Event->button.button){		
		case SDL_BUTTON_LEFT:	
			newField->setFocused(false);
			if( (yClicked >50)&&(yClicked <600 )&&(xClicked >25)&&(xClicked <575 ) ){  // elegxos an clikares ston xarti!
				selected = theSea->getOrganismAt(yOrg,xOrg);
				selectedClass = -1;
			}else if( (yClicked >75)&&(yClicked < 380)&&(xClicked >595)&&(xClicked <895 ) ){
				selectedClass = ((yClicked-75)/60)*2 + (xClicked-595)/150;
				selected = NULL;
			}else if(buttonPlus->Belongs(yClicked,xClicked)){
				if(mode!=MODE_ADDORG)
					ChangeMode(MODE_ADDORG);
				else
					ChangeMode(MODE_NORMAL);
			}else if(buttonContam->Belongs(yClicked,xClicked)){
				if(mode!=MODE_ADDCONTAM)
					ChangeMode(MODE_ADDCONTAM);
				else
					ChangeMode(MODE_NORMAL);
			}else if(buttonNets->Belongs(yClicked,xClicked)){
				if(mode!=MODE_NETS)
					ChangeMode(MODE_NETS);
				else
					ChangeMode(MODE_NORMAL);
			}else if(buttonHelp->Belongs(yClicked,xClicked)){
				buttonHelp->setPushed(true);
				if(!paused)
					Pause();
			}else if(newField->Belongs(yClicked,xClicked)){
				newField->setFocused(true);
			}
			break;	
		case SDL_BUTTON_RIGHT:
			if(mode == MODE_ADDCONTAM){
				if( (yClicked >50)&&(yClicked <600 )&&(xClicked >25)&&(xClicked <575 ) ){
					theSea->addContaminationAt(yOrg,xOrg,theSea->getContaminationAt(yOrg,xOrg) +1);
					if(theSea->getContaminationAt(yOrg,xOrg) == 6)
						theSea->addContaminationAt(yOrg,xOrg,0);
				}
			}else if(mode == MODE_NETS){
				if( (yClicked >50)&&(yClicked <600 )&&(xClicked >25)&&(xClicked <575 ) ){
					if(net1.x==-1 && net1.y==-1){
						net1.x = xOrg;
						net1.y = yOrg;
					}else if(net2.x==-1 && net2.y==-1){
						net2.x = xOrg;
						net2.y = yOrg;
						if(net2.x<net1.x){
							net2.x = net1.x;
							net1.x = xOrg;
						}
						if(net2.y<net1.y){
							net2.y = net1.y;
							net1.y = yOrg;
						}
					}					
				}
			}else if(mode == MODE_ADDORG){
				if( (yClicked >50)&&(yClicked <600 )&&(xClicked >25)&&(xClicked <575 ) ){
					if(selectedClass>=0 && selectedClass<=9 && theSea->getOrganismAt(yOrg,xOrg)==NULL){
						int newAge = 51*newField->GetNumber();				
						
						theSea->createOrganism(yOrg,xOrg,selectedClass,20+newAge*theSea->getStatisticks(selectedClass)->getGrowthRate(),newAge);
					}
				}
			}
			break;
		}

	}
}
void SimulationState::Loop(){
	if( (!paused)&&( SDL_GetTicks() > lastUpdate + timeDelay ) ){
		lastUpdate = SDL_GetTicks();
		theSea->update(&selected);
	}
}
void SimulationState::Render(SDL_Surface* screen){
	RenderMap(screen);
	RenderMisc(screen);	
	RenderTop(screen);

	buttonPlus->Render(screen);
	buttonContam->Render(screen);
	buttonNets->Render(screen);
	buttonHelp->Render(screen);
	if(mode == MODE_ADDORG && selectedClass>=0  && selectedClass<=9)
		newField->Render(screen);

	SDL_Flip(screen);
	if(buttonHelp->isPushed()){
		buttonHelp->setPushed(false);
		SDL_Delay(150);
		theSim->ChangeState(STATE_HELP);
	}
}
void SimulationState::CleanUp(){
	delete camera;
	delete theSea;
	
	SDL_FreeSurface(mapSurf);
	SDL_FreeSurface(miscSurf);
	SDL_FreeSurface(topSurf);
	SDL_FreeSurface(fishes);
	SDL_FreeSurface(tiles);			
	SDL_FreeSurface(nets);    
	SDL_FreeSurface(fishInfo);
	SDL_FreeSurface(classInfo);

	TTF_CloseFont(dateFont);
	TTF_CloseFont(infoFont);
	TTF_CloseFont(numsFont);
	
	delete buttonPlus;
	delete buttonContam;
	delete buttonNets;
	delete buttonHelp;
	delete newField;
}

void SimulationState::RenderTop(SDL_Surface* screen){
	SurfaceLib::Draw(screen,topSurf,25,20);
	SDL_Color dateColor = {255, 255, 255};
	
	dateSurf = TTF_RenderText_Blended(dateFont,theSea->getDate().c_str(),dateColor);
	SurfaceLib::Draw(screen,dateSurf,30,20);	
	SDL_FreeSurface(dateSurf);	
	}
void SimulationState::RenderMap(SDL_Surface* screen){
	std::stringstream tempStream;
	SDL_Color numsColor = {0,0,0};
	SurfaceLib::Draw(screen,mapSurf,0,0);
	
	if(mode == MODE_ADDORG){
		tempStream << "Choose a Category and Right-Click to Add an Organism" ;
		infoMsg = TTF_RenderText_Blended(numsFont,tempStream.str().c_str(), numsColor);
		SurfaceLib::Draw(screen,infoMsg,24,5);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);
	}else if(mode == MODE_ADDCONTAM){
		tempStream << "Right-Click to Add a Pollution Source or Increase it' s Level" ;
		infoMsg = TTF_RenderText_Blended(numsFont,tempStream.str().c_str(), numsColor);
		SurfaceLib::Draw(screen,infoMsg,24,5);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);
	}else if(mode == MODE_NETS){
		tempStream << "Right-Click in 2 spots to Throw Nets in that Area. Then Press ENTER to pull them up or ESC to cancel." ;
		infoMsg = TTF_RenderText_Blended(numsFont,tempStream.str().c_str(), numsColor);
		SurfaceLib::Draw(screen,infoMsg,24,5);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);
	}

	for(int i=0;i<TILES_IN_Y;i++){
		int I = camera->getY() - 5 + i;
		
		tempStream << I ;
		infoMsg = TTF_RenderText_Solid(numsFont,tempStream.str().c_str(), numsColor);
		SurfaceLib::Draw(screen,infoMsg,1,i*50+65);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		tempStream << camera->getX() - 5 + i ;
		infoMsg = TTF_RenderText_Solid(numsFont,tempStream.str().c_str(), numsColor);
		SurfaceLib::Draw(screen,infoMsg,50*i+40,602);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);
		
		for(int j=0;j<TILES_IN_X;j++){			
			int J = camera->getX() - 5 + j;
			
			SurfaceLib::Draw(mapSurf,tiles,j*TILE_SIZE+25,i*TILE_SIZE+50,theSea->getContaminationAt(I,J)*TILE_SIZE,0,TILE_SIZE,TILE_SIZE);	
			if( theSea->getOrganismAt(I,J)!=NULL){
				SurfaceLib::Draw(mapSurf,fishes,j*TILE_SIZE+25,i*TILE_SIZE+50, (theSea->getOrganismAt(I,J)->getAttr()->getID())*TILE_SIZE,0,TILE_SIZE,TILE_SIZE);	
			}
			if(mode==MODE_NETS && net2.y!=-1 && hasNets(I,J)){
				SurfaceLib::Draw(mapSurf,nets,j*TILE_SIZE+25,i*TILE_SIZE+50);
			}
		}
	}
}
void SimulationState::RenderMisc(SDL_Surface* screen){
	SurfaceLib::Draw(screen,miscSurf,595,0);
	RenderMiscTop(screen);
	RenderMiscBot(screen);	
}
void SimulationState::RenderMiscTop(SDL_Surface* screen){
	for(int y=0;y<=4;y++){
		for(int x=0;x<=1;x++){
			SurfaceLib::Draw(miscSurf,fishes,x*150+4,y*62+79,(y*2+x)*TILE_SIZE,0,TILE_SIZE,TILE_SIZE);		
		}	
	}

}
void SimulationState::RenderMiscBot(SDL_Surface* screen){
	SDL_Color infoColor = {255, 255, 255};
	std::stringstream tempStream;
	int w = 745;
	int h = 455;		  
	if( selected!= NULL){	
		SurfaceLib::Draw(screen,fishInfo,595,400);	
 		SurfaceLib::Draw(screen,fishes,720,400, (selected->getAttr()->getID())*TILE_SIZE,0,TILE_SIZE,TILE_SIZE);	
					
		infoMsg = TTF_RenderText_Solid(infoFont,selected->getName().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,w,h);
		SDL_FreeSurface(infoMsg);
			
		tempStream << selected->getAge()/51 ;
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,w,h+20);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		tempStream << selected->getSize() ;
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,w,h+40);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);
		
		tempStream << selected->getPosition().x ;
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,w+32,h+60);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		tempStream << selected->getPosition().y ;
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,w+98,h+60);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);
			
		if(selected->getAttr()->getID()!=0)
			tempStream << selected->getSize()/10;
		else
			tempStream << "-";
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,w,h+80);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		if(selected->getAttr()->getID()!=0)
			tempStream << selected->getFoodEaten() ;
		else
			tempStream << "-";
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,w,h+100);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		tempStream << selected->getAttr()->getGrowthRate();
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,w,h+120);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);			
	}else if(selectedClass>=0 && selectedClass<=9){
		SurfaceLib::Draw(screen,classInfo,595,400);	
		SurfaceLib::Draw(screen,fishes,720,400, (selectedClass)*TILE_SIZE,0,TILE_SIZE,TILE_SIZE);
		Attributes* atrTemp =  theSea->getStatisticks(selectedClass);
		
		tempStream << atrTemp->getPopulation();
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,800,h);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		tempStream << std::setiosflags(std::ios::fixed) << std::setw(5) <<std::setprecision(2) << atrTemp->getDeathPercentage()<<"%";
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,800,h+23);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		tempStream << std::setw(5)<< atrTemp->getAgeAv();
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,800,h+46);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		tempStream << std::setw(5)<< atrTemp->getSizeAv();
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,800,h+69);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		tempStream << std::setw(5)<< atrTemp->getFoodAv();
		infoMsg = TTF_RenderText_Solid(infoFont,tempStream.str().c_str(), infoColor);
		SurfaceLib::Draw(screen,infoMsg,800,h+92);
		tempStream.str("");
		SDL_FreeSurface(infoMsg);

		if(mode==MODE_ADDORG){
			tempStream << "Organism's Age:";
			infoMsg = TTF_RenderText_Blended(infoFont,tempStream.str().c_str(), infoColor);
			SurfaceLib::Draw(screen,infoMsg,605,578);
			tempStream.str("");
			SDL_FreeSurface(infoMsg);
		}
	}
}

void SimulationState::Pause(){
	std::stringstream tempStream;
	if(paused){
		paused = false;
		SDL_WM_SetCaption(theSim->GetWinName(),"i");
	}else{
		paused = true;
		tempStream<< theSim->GetWinName()<<"   (Paused)";
		SDL_WM_SetCaption(tempStream.str().c_str(),"i");
	}
}
void SimulationState::ChangeMode(int tmode){
	if( tmode<0 || tmode>4) return;
	mode = tmode;
	if(!paused)
		Pause();
	buttonPlus->setPushed(false);
	buttonContam->setPushed(false);
	buttonNets->setPushed(false);
	
	switch(tmode){
	case MODE_NORMAL:	
		break;
	case MODE_ADDORG:
		buttonPlus->setPushed(true);		
		break;
	case MODE_ADDCONTAM:
		buttonContam->setPushed(true);		
		break;
	case MODE_NETS:
		buttonNets->setPushed(true);
		net1.x = net1.y = net2.x = net2.y = -1;
		break;
	}

}
bool SimulationState::hasNets(int y,int x){
	if(y>=net1.y && y<=net2.y && x>=net1.x && x<=net2.x)
		return true;

	return false;
}

void SimulationState::Restart(){
	delete theSea;
	delete camera;
	theSim->ChangeState(STATE_INTRO);
}