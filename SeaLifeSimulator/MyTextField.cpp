# include  "MyTextField.h"

My_SDL_TextField::My_SDL_TextField(int ty,int tx,int theight,int twidth){
	y = ty;
	x = tx;
	height = theight;
	width = twidth;
	focused = false;
	textStream.str("");
	maxDigits = 4;

	textSurf = NULL;
	textFont = NULL;
}

My_SDL_TextField::~My_SDL_TextField(){
	SDL_FreeSurface(textSurf);
	TTF_CloseFont(textFont);
}

void My_SDL_TextField::setFocused(bool tpushed){
	focused = tpushed;
}
bool My_SDL_TextField::isFocused(){return focused;}

int My_SDL_TextField::GetNumber(){
	int num=0;
	num = atoi(textStream.str().c_str());
	return num;
}

void My_SDL_TextField::AddText(SDL_Event* keyEve){
	std::string temp;
	if(textStream.str().length() >= maxDigits && keyEve->key.keysym.sym!=SDLK_BACKSPACE ) return;
	if(keyEve->type!=SDL_KEYDOWN) return;
		switch(keyEve->key.keysym.sym) {		
		case SDLK_0 :
		case SDLK_KP0:
			textStream << "0";
			break;
		case SDLK_1 :
		case SDLK_KP1:
			textStream << "1";
			break;
		case SDLK_2 :
		case SDLK_KP2:
			textStream << "2";
			break;
		case SDLK_3 :
		case SDLK_KP3:
			textStream << "3";
			break;
		case SDLK_4 :
		case SDLK_KP4:
			textStream << "4";
			break;
		case SDLK_5 :
		case SDLK_KP5:
			textStream << "5";
			break;
		case SDLK_6 :
		case SDLK_KP6:
			textStream << "6";
			break;
		case SDLK_7 :
		case SDLK_KP7:
			textStream << "7";
			break;
		case SDLK_8 :
		case SDLK_KP8:
			textStream << "8";
			break;
		case SDLK_9 :
		case SDLK_KP9:
			textStream << "9";
			break;
		case SDLK_BACKSPACE:
			temp = textStream.str().substr(0,textStream.str().length()-1);
			textStream.str("");
			textStream << temp;
			break;
		}
}
bool My_SDL_TextField::Load(char* imageName,char* fontName,int fontSize){
	textSurf = SurfaceLib::LoadImg(imageName);
	if(textSurf == NULL) {
		return false;
	}	
	if( (textFont = TTF_OpenFont(fontName, fontSize)) == NULL){
		return false;
	}
	return true;
}
bool My_SDL_TextField::Belongs(int yClicked,int xClicked){
	if( (yClicked > y)&&(yClicked < y+height)&&(xClicked > x)&&(xClicked < x+width) ){
		return true;
	}
	return false;
}
void My_SDL_TextField::Render(SDL_Surface* container){
	SDL_Surface* tempSurf = NULL;
	SDL_Color textColor = {0,0,0};
	if(container==NULL) return;
	if(!focused && textStream.str().compare("")==0)
		SurfaceLib::Draw(container,textSurf,x,y,0,0,width,height);
	else
		SurfaceLib::Draw(container,textSurf,x,y,0,height,width,height);
	
	tempSurf = TTF_RenderText_Blended(textFont,textStream.str().c_str(),textColor);
	SurfaceLib::Draw(container,tempSurf,x+5,y+1);
	SDL_FreeSurface(tempSurf);
}