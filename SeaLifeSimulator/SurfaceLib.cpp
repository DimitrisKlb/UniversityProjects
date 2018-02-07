#include "SurfaceLib.h"
 
SurfaceLib::SurfaceLib() {
}
 
SDL_Surface* SurfaceLib::LoadImg(char* file) {
    SDL_Surface* surfTemp = NULL;
    SDL_Surface* surfFinal = NULL;
 
    if((surfTemp = IMG_Load(file)) == NULL) {
        return NULL;
    }
 
    surfFinal = SDL_DisplayFormat(surfTemp);
    SDL_FreeSurface(surfTemp);
 
    return surfFinal;
}

bool SurfaceLib::Draw(SDL_Surface* surfDest, SDL_Surface* surfSrc, int x, int y, int x2, int y2, int w, int h){
    if(surfDest == NULL || surfSrc == NULL) {
        return false;
    }
 
    SDL_Rect DestR;
 
    DestR.x = x;
    DestR.y = y;
 
    SDL_Rect SrcR;
 
    SrcR.x = x2;
    SrcR.y = y2;
    SrcR.w = w;
    SrcR.h = h;
 
    SDL_BlitSurface(surfSrc, &SrcR, surfDest, &DestR);
 
    return true;
}

bool SurfaceLib::Draw(SDL_Surface* surfDest, SDL_Surface* surfSrc, int x, int y) {
	if(surfDest == NULL || surfSrc == NULL) {
        return false;
    }
    return SurfaceLib::Draw(surfDest,surfSrc,x,y,0,0,surfSrc->w,surfSrc->h);
}

bool SurfaceLib::Transparent(SDL_Surface* surfDest, int R, int G, int B) {
    if(surfDest == NULL) {
        return false;
    }
 
    SDL_SetColorKey(surfDest, SDL_SRCCOLORKEY, SDL_MapRGB(surfDest->format, R, G, B));
 
    return true;
}