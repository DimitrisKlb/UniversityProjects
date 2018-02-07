#ifndef CAMERA_LIB_H
    #define CAMERA_LIB_H
 
#include <SDL.h>

#include "GraphicDefines.h"
 
class CameraHandler{
 
    private:
        int y,x;
		int minX,minY;
		int maxX,maxY;
 
    public:
        CameraHandler(int=11,int=11);
 
        int getX();
        int getY();
        void setPos(int Y, int X);
 
		void move(int dy, int dx);
};
 
#endif