#ifndef NAMES_H
#define NAMES_H

#include <cstdlib>
#include <ctime>
#include <sstream>

#include "student.h"

void getRandomStudent(Student* &randomStudent, int &previousAM);

const int lastNamesNum = 13;
const int firstNamesNum = 16;
char* lastNames[] = {
	"Kallibwkas"		,
	"Papadopoulos"		,
	"Papalabropoulos"	,
	"Kostantinou"		,
	"Sakellarios"		,
	"Flamiatos"			,
	"Georgiadis"		,
	"Kouremenos"		,
	"Katsaros"			,
	"Papadatos"			,
	"Kostopoulos"		,
	"Georgiadis"		,
	"Mponos"
};
char* firstNames[] = {
	"Alexis"		,
	"Baggelis"		,
	"Dimitris"		,
	"Fotis"			,
	"Gerasimos"		,
	"Kostas"		,
	"Giorgos"		,
	"Giannis"		,
	"Labros"		,
	"Petros"		,
	"Pantelis"		,
	"Manolis"		,
	"Hlias"			,
	"Thanasis"		,
	"Tasos"			,
	"Thoukididis"	
};

void getRandomStudent(Student* &randomStudent, int &previousAM){	
	static bool firstTime = true;
	if(firstTime == true){
		srand((unsigned int)time(NULL));
		firstTime = false;
	}
	std::stringstream rAM;
	previousAM += rand()%10 + 1;
	rAM << previousAM;
	int rLN = rand() % lastNamesNum;
	int rFN = rand() % firstNamesNum;
	std::stringstream rYear;
	rYear << rand() % 10 + 1; 
	std::stringstream rAge;
	rAge << rand() % 12 + 18;
	
	randomStudent = new Student((char*)rAM.str().c_str(), lastNames[rLN], 
		firstNames[rFN], (char*)rYear.str().c_str(), (char*)rAge.str().c_str());
}

#endif // NAMES_H