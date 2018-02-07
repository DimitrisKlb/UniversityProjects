#include "Simulator.h"

//Dimiourgia enos basikou antikeimenou Simulator kai ekinnisi tou para8irou-prosomoiwsis

int main( int argc, char* args[] ) {  
	Simulator sim("Sea Life Simulator - 1.2.0",WIN_HEIGHT,WIN_WIDTH);
	return sim.Execute();
}