#include "CameraLib.h"


 CameraHandler::CameraHandler(int seaY,int seaX) {
	minY = TILES_IN_Y/2;
	minX = TILES_IN_X/2;
	maxY = seaY - TILES_IN_Y/2 - 1;
	maxX = seaX - TILES_IN_X/2 - 1;
	y = 5;
	x = 5;
}
 
void CameraHandler::move(int dy, int dx) {
	if( (y+dy>=minY)&&(x+dx>=minX)&&(y+dy<=maxY)&&(x+dx<=maxX) ){
		y += dy;
		x += dx;
	}
}
 
int CameraHandler::getX(){ 
    return x;
}
 int CameraHandler::getY() { 
    return y;
}
 
void CameraHandler::setPos(int Y, int X) {
    y = Y;
	x = X;
}