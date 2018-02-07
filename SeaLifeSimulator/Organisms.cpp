#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include "Organisms.h"
using namespace std;

Organism::Organism(string tname,int tsize,int tage,int tDyingAge)
	:name(tname),size(tsize),age(tage),dyingAge(tDyingAge)
{
	contaminationLvl = 0;
	acted = false;
	position.x = -1;
	position.y = -1;

	foodEaten=0;
	canReproduce = true;
}
void Organism::setName(string tname){
	name = tname;
}
void Organism::setAge(int tage){
	age = tage;
}
void Organism::setDyingAge(int tDyingAge){
	dyingAge = tDyingAge;
}
void Organism::setSize(int tsize){
	size = tsize;
}
void Organism::setPosition(int newY,int newX){
	position.y = newY;
	position.x = newX;
}
void Organism::setContamLvl(int contamLvl){
 	contaminationLvl = contamLvl;
	dyingAge -= (int) pow(5.0,contaminationLvl) + 200;
}
void Organism::setActed(bool tacted){acted = tacted;}
void Organism::setFoodEaten(int tfoodEaten){ 
	foodEaten = tfoodEaten;
}
void Organism::setCanReproduce(bool tcanReproduce){
	canReproduce = tcanReproduce;
}

string Organism::getName(){return name;}
int Organism::getAge(){return age;}
int Organism::getDyingAge(){return dyingAge;}
int Organism::getSize(){return size;}
Point Organism::getPosition(){return position;}
int Organism::getContamLvl(){return contaminationLvl;}
bool Organism::hasActed(){return acted;}
int Organism::getFoodEaten(){ return foodEaten;}
bool Organism::CanReproduce(){return canReproduce;}

bool Organism::isHungry(){
	if (foodEaten >= size/10)
		return false;
	return true;
}
void Organism::eat(Organism target){     
	this->foodEaten += target.getSize();
	if(target.contaminationLvl>this->contaminationLvl){
		this->contaminationLvl = target.contaminationLvl;
	}
}
Attributes* Organism::getAttr(){
	return NULL;
}

Attributes* PhytoPlankton::att = new Attributes(PHYTO_ID,0,PHYTO_FLEVEL,PHYTO_GROWTHR,PHYTO_MFREQ,PHYTO_MAXSIZE);
Attributes* ZooPlankton::att = new Attributes(ZOO_ID,0,ZOO_FLEVEL,ZOO_GROWTHR,ZOO_MFREQ,ZOO_MAXSIZE);
Attributes* Sardine::att = new Attributes(SARDINE_ID,0,SARDINE_FLEVEL,SARDINE_GROWTHR,SARDINE_MFREQ,SARDINE_MAXSIZE);
Attributes* Shrimp::att = new Attributes(SHRIMP_ID,0,SHRIMP_FLEVEL,SHRIMP_GROWTHR,SHRIMP_MFREQ,SHRIMP_MAXSIZE);
Attributes* Squid::att = new Attributes(SQUID_ID,0,SQUID_FLEVEL,SQUID_GROWTHR,SQUID_MFREQ,SQUID_MAXSIZE);
Attributes* Tuna::att = new Attributes(TUNA_ID,0,TUNA_FLEVEL,TUNA_GROWTHR,TUNA_MFREQ,TUNA_MAXSIZE);
Attributes* WolfEel::att = new Attributes(WOLFEEL_ID,0,WOLFEEL_FLEVEL,WOLFEEL_GROWTHR,WOLFEEL_MFREQ,WOLFEEL_MAXSIZE);
Attributes* Octopus::att = new Attributes(OCTOPUS_ID,0,OCTOPUS_FLEVEL,OCTOPUS_GROWTHR,OCTOPUS_MFREQ,OCTOPUS_MAXSIZE);
Attributes* Dolphin::att = new Attributes(DOLPHIN_ID,0,DOLPHIN_FLEVEL,DOLPHIN_GROWTHR,DOLPHIN_MFREQ,DOLPHIN_MAXSIZE);
Attributes* Shark::att = new Attributes(SHARK_ID,0,SHARK_FLEVEL,SHARK_GROWTHR,SHARK_MFREQ,SHARK_MAXSIZE);

Plankton::Plankton(){}
Plankton::Plankton(string tname,int tsize,int tage,int tdyingAge)
	:Organism(tname,tsize,tage,tdyingAge)
{
}
Attributes* Plankton::getAttr(){
	return NULL;
}

NonPlanktonic::NonPlanktonic(){}
NonPlanktonic::NonPlanktonic(string tname,int tsize,int tage,int tdyingAge)
	:Organism(tname,tsize,tage,tdyingAge)
{
}
Attributes* NonPlanktonic::getAttr(){
	return NULL;
}

PhytoPlankton::PhytoPlankton(int tsize,int tage,string tname)
	:Plankton(tname,tsize,tage,PHYTO_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* PhytoPlankton::getAttr(){
	return att;
}

ZooPlankton::ZooPlankton(int tsize,int tage,string tname)
	:Plankton(tname,tsize,tage,ZOO_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* ZooPlankton::getAttr(){
	return att;
}

Sardine::Sardine(int tsize,int tage,string tname)
	:NonPlanktonic(tname,tsize,tage,SARDINE_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* Sardine::getAttr(){
	return att;
}

Shrimp::Shrimp(int tsize,int tage,string tname)
	:NonPlanktonic(tname,tsize,tage,SHRIMP_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* Shrimp::getAttr(){
	return att;
}

Squid::Squid(int tsize,int tage,string tname)
	:NonPlanktonic(tname,tsize,tage,SQUID_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* Squid::getAttr(){
	return att;
}

Tuna::Tuna(int tsize,int tage,string tname)
	:NonPlanktonic(tname,tsize,tage,TUNA_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* Tuna::getAttr(){
	return att;
}

WolfEel::WolfEel(int tsize,int tage,string tname)
	:NonPlanktonic(tname,tsize,tage,WOLFEEL_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* WolfEel::getAttr(){
	return att;
}

Octopus::Octopus(int tsize,int tage,string tname)
	:NonPlanktonic(tname,tsize,tage,OCTOPUS_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* Octopus::getAttr(){
	return att;
}

Dolphin::Dolphin(int tsize,int tage,string tname)
	:NonPlanktonic(tname,tsize,tage,DOLPHIN_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* Dolphin::getAttr(){
	return att;
}

Shark::Shark(int tsize,int tage,string tname)
	:NonPlanktonic(tname,tsize,tage,SHARK_DAGE)
{
	att->setPopulation(att->getPopulation()+1);
}
Attributes* Shark::getAttr(){
	return att;
}


Attributes::Attributes(){}
Attributes::Attributes(int tID,int tpopulation,int tfoodLevel,int tgrowthRate,int tmovementFrequency,int tmaxSize){
	ID = tID;
	population = tpopulation;
	foodLevel = tfoodLevel;
	growthRate = tgrowthRate;
	movementFrequency = tmovementFrequency;
	maxSize = tmaxSize;

	sizeAv = 0.0;
	foodAv = 0.0;
	ageAv = 0.0;
	deaths = 0;
	deathPercent = 0.0;
}
void Attributes::setPopulation(int tpopulation){
	population = tpopulation;
}
void Attributes::setSizeAv(float tsizeAv){
	sizeAv = tsizeAv;
}
void Attributes::setFoodAv(float tfoodAv){
	foodAv = tfoodAv;
}
void Attributes::setAgeAv(float tageAv){
	ageAv = tageAv;
}
void Attributes::setDeaths(int tdeaths){
	deaths = tdeaths;
}
void Attributes::setDeathPercentage(float tdeathPercent){deathPercent = tdeathPercent;}

int Attributes::getID(){return ID;}
int Attributes::getPopulation(){ return population;}
int Attributes::getFoodLevel(){return foodLevel;}
int Attributes::getGrowthRate(){return growthRate;}
int Attributes::getMovementFreq(){return movementFrequency;}
int Attributes::getMaxSize(){return maxSize;}
float Attributes::getSizeAv(){return sizeAv;}
float Attributes::getFoodAv(){return foodAv;}
float Attributes::getAgeAv(){return ageAv;}
int Attributes::getDeaths(){return deaths;}
float Attributes::getDeathPercentage(){return deathPercent;}







