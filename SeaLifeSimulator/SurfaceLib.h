#ifndef SURFACE_LIB
    #define SURFACE_LIB
 
#include <SDL.h>
#include <SDL_image.h>
 
class SurfaceLib {
    public:
        SurfaceLib();
 
    public:
        static SDL_Surface* LoadImg(char* file);
		static bool Draw(SDL_Surface* surfDest, SDL_Surface* surfSrc, int x, int y, int x2, int y2, int w, int h);
		static bool Draw(SDL_Surface* surfDest, SDL_Surface* surfSrc, int x, int y);
		static bool Transparent(SDL_Surface* surfDest, int R, int G, int B);
};
 
#endif