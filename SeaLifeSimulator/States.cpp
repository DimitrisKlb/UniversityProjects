#include "States.h"

State::State(){
	theSim = NULL;
}

void State::SetSimulator(Simulator* sim){
	theSim = sim;
}
	
Simulator* State::GetSimulator(){
	return theSim;
}



