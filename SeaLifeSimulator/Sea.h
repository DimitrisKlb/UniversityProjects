#ifndef SEA_H
#define SEA_H

#include "Organisms.h"

class Sea{
public:
	Sea(int y=11,int x=11,bool random=false);
	int getWidth();
	int getHeight();
	int getContaminationAt(int y,int x);
	Organism* getOrganismAt(int y,int x);
	string getDate();
	void addContaminationAt(int y,int x,int contamLvl);
	void addOrganism(int y,int x,Organism* org);

	void update(Organism** selected);                                     // Basiki me8odos tis klasis kai tis prosomoiwsis pou proxwraei ton kosmos kata misi mera
 
	void createOrganism(int y,int x,int fishType,int size,int age);
	Attributes* getStatisticks(int fishType);                              // Epistrofi tou data member Attributes sigkekrimenou eidous
	void throwNets(int y1,int x1,int y2,int x2);                           // Ripsi diktiwn se sigkekrimeni perioxi 
	~Sea();

private:  // boi8itikes me8odoi gia kaliteri organosi
	inline void updateMap();
	inline void updateTime();
	inline void updateStatisticks();        
	inline Point canEat(int,int);             // Elegxei an iparxei trofi girw apo ton sigkerkimeno oragnismo
	inline Point canReproduce(int I,int J);   // Elegxei an o sigkekrimenos oragnismos mporei na anaparax8ei
	inline void die(int I,int J);             // Skotonei ton oragnismo sto sigkekrimeno keli
	
private:
	int width,height;
	Organism*** map;                          // H anaparastasi tis 8alassas 
	int** contamination;                      // Parallilos pinakas pou apo8ikeuei ta epipeda molinsis tou xarti
	int hours12,days,weeks,years;             // Imerominia

	Organism* selectedOrg;                    // Epilegmenos organismos
};

#endif
