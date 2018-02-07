#ifndef ORGANISMS_H
#define ORGANISMS_H
#include <string>

#include "FishDefines.h"
using std::string;


struct Point{
	int y,x;
};
class Attributes{							 // Boi8itiki klasi me tis aparaitites metablites tou prepei na exei monadika(static) ka8e telikos organismos
public:
	Attributes();
	Attributes(int,int,int,int,int,int);
	void setPopulation(int);
	void setSizeAv(float);
	void setFoodAv(float);
	void setAgeAv(float);
	void setDeaths(int);
	void setDeathPercentage(float);

	int getID();
	int getPopulation();
	int getFoodLevel();
	int getGrowthRate();
	int getMovementFreq();
	int getMaxSize();
	float getSizeAv();
	float getFoodAv();
	float getAgeAv();
	int getDeaths();
	float getDeathPercentage();

private:
	int ID;								// Monadiko diaxwristiki tis ka8e telikis ipoklasis organismou
	int population;						// Pli8ismos
	int foodLevel;                      // Diatrofiko epipedo stin diatrofiki alisida
	int growthRate;                     // Ba8mos anapti3is
	int movementFrequency;              // Sixnotita kinisis  
	int maxSize;
//Variables needed for statistics
	float sizeAv;                       // Meso mege8os
	float foodAv;						// Mesi katanalwsi teleutaias ebdomadas			
	float ageAv;						// Mesi Ilikia
	int deaths;							// Thanatoi
	float deathPercent;					// Pososto 8anatwn
};

class Organism{
public:
	Organism(string tname="Organism",int tsize=1,int tage=0,int dyingAge=0);	
	Organism( int tsize,int tage,int tdyingAge,string tname,int num);

	void setName(string);
	void setAge(int);
	void setDyingAge(int);
	void setSize(int);
	void setPosition(int newY,int newX);
	void setContamLvl(int contLvl);
	void setActed(bool tacted);
	void setFoodEaten(int tfoodEaten);
	void setCanReproduce(bool tcanReproduce);
	
	string getName();
	int getAge();
	int getDyingAge();
	int getSize();
	Point getPosition();
	int getContamLvl();
	bool hasActed();
	int getFoodEaten();
	bool CanReproduce();

	void eat(Organism);
	bool isHungry();

	virtual Attributes* getAttr();
private:
	string name;              // Onoma
	int age;                  // Ilikia (metroumeni se ebdomades) 
	int dyingAge;			  // Ilikia 8anatou
	int size;                 // Megethos 
	Point position;           // Thesi se disdiastato xarti
	int contaminationLvl;     // Epipedo molinsis
	bool acted;               // Metabliti pou deixnei an exei kouni8ei o oragnismos

	int foodEaten;			  // Trofi pou katanalw8ike tin teleutaia ebdomada	 
	bool canReproduce;        // Metabliti pou deixnei an exei tin dinatotita na anaparax8ei tin sigkekrimeni stigmi
};              // Base class

class Plankton:public Organism{
public:
	Plankton();
	Plankton(string tname,int tsize,int tage,int tdyingAge);

	virtual Attributes* getAttr();
};      
class PhytoPlankton:public Plankton{
public:
	PhytoPlankton(int tsize=0,int tage=0,string tname="PhytoPlankton");
	Attributes* getAttr();
	static Attributes* att;   
};
class ZooPlankton:public Plankton{
public:
	ZooPlankton(int tsize,int tage,string tname="ZooPlankton");
	Attributes* getAttr();
	static Attributes* att;
};

class NonPlanktonic:public Organism{
public:
	NonPlanktonic();
	NonPlanktonic(string tname,int tsize,int tage,int tDyingAge);

	virtual Attributes* getAttr();
};
class Sardine:public NonPlanktonic{
public:
	Sardine(int tsize=0,int tage=0,string tname="Sardine");
	Attributes* getAttr();
	static Attributes* att;
};
class Shrimp:public NonPlanktonic{
public:
	Shrimp(int tsize=0,int tage=0,string tname="Shrimp");
	Attributes* getAttr();
	static Attributes* att;
};
class Squid:public NonPlanktonic{
public:
	Squid(int tsize=0,int tage=0,string tname="Squid");
	Attributes* getAttr();
	static Attributes* att;
};
class Tuna:public NonPlanktonic{
public:
	Tuna(int tsize=0,int tage=0,string tname="Tuna");
	Attributes* getAttr();
	static Attributes* att;
};
class WolfEel:public NonPlanktonic{
public:
	WolfEel(int tsize=0,int tage=0,string tname="WolfEel");
	Attributes* getAttr();
	static Attributes* att;
};
class Octopus:public NonPlanktonic{
public:
	Octopus(int tsize=0,int tage=0,string tname="Octopus");
	Attributes* getAttr();
	static Attributes* att;
};
class Dolphin:public NonPlanktonic{
public:
	Dolphin(int tsize=0,int tage=0,string tname="Dolphin");
	Attributes* getAttr();
	static Attributes* att;
};
class Shark:public NonPlanktonic{
public:
	Shark(int tsize=0,int tage=0,string tname="Shark");
	Attributes* getAttr();
	static Attributes* att;
};


#endif